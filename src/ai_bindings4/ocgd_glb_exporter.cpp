#include "ocgd_glb_exporter.h"
#include "ocgd_shape.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/error_macros.hpp>

// OpenCASCADE includes
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <BRep_Tool.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <TopLoc_Location.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>
#include <Quantity_Color.hxx>
#include <RWGltf_CafWriter.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <TDF_Label.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Failure.hxx>

using namespace godot;

ocgd_glb_exporter::ocgd_glb_exporter() {
    mesh_deflection = 0.1;
    angular_deflection = 0.5;
    export_colors = true;
    export_materials = true;
    export_textures = false;
    merge_faces = false;
    texture_size = 512;
    default_material_name = "Material";
    mesh_optimization = false;
    last_error = "";
    shape_colors = Dictionary();
    shape_materials = Dictionary();
}

ocgd_glb_exporter::~ocgd_glb_exporter() {
}

void ocgd_glb_exporter::_bind_methods() {
    ClassDB::bind_static_method("ocgd_glb_exporter", D_METHOD("new_exporter"), &ocgd_glb_exporter::new_exporter);
    
    ClassDB::bind_method(D_METHOD("export_shape", "shape", "file_path"), &ocgd_glb_exporter::export_shape);
    ClassDB::bind_method(D_METHOD("export_shapes", "shapes", "file_path"), &ocgd_glb_exporter::export_shapes);
    ClassDB::bind_method(D_METHOD("export_assembly", "shapes", "transforms", "file_path"), &ocgd_glb_exporter::export_assembly);
    ClassDB::bind_method(D_METHOD("export_shape_with_options", "shape", "file_path", "options"), &ocgd_glb_exporter::export_shape_with_options);
    ClassDB::bind_method(D_METHOD("export_assembly_with_options", "shapes", "transforms", "file_path", "options"), &ocgd_glb_exporter::export_assembly_with_options);
    
    ClassDB::bind_method(D_METHOD("set_mesh_deflection", "deflection"), &ocgd_glb_exporter::set_mesh_deflection);
    ClassDB::bind_method(D_METHOD("get_mesh_deflection"), &ocgd_glb_exporter::get_mesh_deflection);
    ClassDB::bind_method(D_METHOD("set_angular_deflection", "deflection"), &ocgd_glb_exporter::set_angular_deflection);
    ClassDB::bind_method(D_METHOD("get_angular_deflection"), &ocgd_glb_exporter::get_angular_deflection);
    
    ClassDB::bind_method(D_METHOD("set_export_colors", "enable"), &ocgd_glb_exporter::set_export_colors);
    ClassDB::bind_method(D_METHOD("get_export_colors"), &ocgd_glb_exporter::get_export_colors);
    ClassDB::bind_method(D_METHOD("set_export_materials", "enable"), &ocgd_glb_exporter::set_export_materials);
    ClassDB::bind_method(D_METHOD("get_export_materials"), &ocgd_glb_exporter::get_export_materials);
    ClassDB::bind_method(D_METHOD("set_export_textures", "enable"), &ocgd_glb_exporter::set_export_textures);
    ClassDB::bind_method(D_METHOD("get_export_textures"), &ocgd_glb_exporter::get_export_textures);
    ClassDB::bind_method(D_METHOD("set_merge_faces", "enable"), &ocgd_glb_exporter::set_merge_faces);
    ClassDB::bind_method(D_METHOD("get_merge_faces"), &ocgd_glb_exporter::get_merge_faces);
    ClassDB::bind_method(D_METHOD("set_texture_size", "size"), &ocgd_glb_exporter::set_texture_size);
    ClassDB::bind_method(D_METHOD("get_texture_size"), &ocgd_glb_exporter::get_texture_size);
    ClassDB::bind_method(D_METHOD("set_default_material_name", "name"), &ocgd_glb_exporter::set_default_material_name);
    ClassDB::bind_method(D_METHOD("get_default_material_name"), &ocgd_glb_exporter::get_default_material_name);
    
    ClassDB::bind_method(D_METHOD("get_export_info"), &ocgd_glb_exporter::get_export_info);
    ClassDB::bind_method(D_METHOD("get_mesh_statistics"), &ocgd_glb_exporter::get_mesh_statistics);
    ClassDB::bind_method(D_METHOD("get_exported_triangle_count"), &ocgd_glb_exporter::get_exported_triangle_count);
    ClassDB::bind_method(D_METHOD("get_exported_vertex_count"), &ocgd_glb_exporter::get_exported_vertex_count);
    
    ClassDB::bind_method(D_METHOD("add_shape_color", "shape", "color"), &ocgd_glb_exporter::add_shape_color);
    ClassDB::bind_method(D_METHOD("add_shape_material", "shape", "material"), &ocgd_glb_exporter::add_shape_material);
    ClassDB::bind_method(D_METHOD("clear_shape_colors"), &ocgd_glb_exporter::clear_shape_colors);
    ClassDB::bind_method(D_METHOD("clear_shape_materials"), &ocgd_glb_exporter::clear_shape_materials);
    
    ClassDB::bind_method(D_METHOD("export_hierarchical_assembly", "assembly_tree", "file_path"), &ocgd_glb_exporter::export_hierarchical_assembly);
    ClassDB::bind_method(D_METHOD("create_assembly_node", "name", "shape", "transform"), &ocgd_glb_exporter::create_assembly_node);
    
    ClassDB::bind_method(D_METHOD("set_coordinate_system_conversion", "from_system", "to_system"), &ocgd_glb_exporter::set_coordinate_system_conversion);
    ClassDB::bind_method(D_METHOD("set_unit_scale", "scale"), &ocgd_glb_exporter::set_unit_scale);
    ClassDB::bind_method(D_METHOD("set_y_up_conversion", "enable"), &ocgd_glb_exporter::set_y_up_conversion);
    
    ClassDB::bind_method(D_METHOD("get_last_error"), &ocgd_glb_exporter::get_last_error);
    ClassDB::bind_method(D_METHOD("has_error"), &ocgd_glb_exporter::has_error);
    ClassDB::bind_method(D_METHOD("clear_error"), &ocgd_glb_exporter::clear_error);
    
    ClassDB::bind_method(D_METHOD("validate_shapes_for_export", "shapes"), &ocgd_glb_exporter::validate_shapes_for_export);
    ClassDB::bind_method(D_METHOD("preview_export", "shapes"), &ocgd_glb_exporter::preview_export);
    
    ClassDB::bind_method(D_METHOD("get_supported_extensions"), &ocgd_glb_exporter::get_supported_extensions);
    ClassDB::bind_method(D_METHOD("get_export_capabilities"), &ocgd_glb_exporter::get_export_capabilities);
    
    ClassDB::bind_method(D_METHOD("get_type"), &ocgd_glb_exporter::get_type);
    ClassDB::bind_method(D_METHOD("is_null"), &ocgd_glb_exporter::is_null);
    
    ClassDB::bind_method(D_METHOD("set_mesh_optimization", "enable"), &ocgd_glb_exporter::set_mesh_optimization);
    ClassDB::bind_method(D_METHOD("get_mesh_optimization"), &ocgd_glb_exporter::get_mesh_optimization);
    ClassDB::bind_method(D_METHOD("set_vertex_welding_tolerance", "tolerance"), &ocgd_glb_exporter::set_vertex_welding_tolerance);
    ClassDB::bind_method(D_METHOD("get_vertex_welding_tolerance"), &ocgd_glb_exporter::get_vertex_welding_tolerance);
}

Ref<ocgd_glb_exporter> ocgd_glb_exporter::new_exporter() {
    return memnew(ocgd_glb_exporter);
}

bool ocgd_glb_exporter::export_shape(const Ref<ocgd_shape>& shape, const String& file_path) {
    Array shapes;
    shapes.append(shape);
    return export_shapes(shapes, file_path);
}

bool ocgd_glb_exporter::export_shapes(const Array& shapes, const String& file_path) {
    Array empty_transforms;
    return export_assembly(shapes, empty_transforms, file_path);
}

bool ocgd_glb_exporter::export_assembly(const Array& shapes, const Array& transforms, const String& file_path) {
    Dictionary options;
    return export_assembly_with_options(shapes, transforms, file_path, options);
}

bool ocgd_glb_exporter::export_shape_with_options(const Ref<ocgd_shape>& shape, const String& file_path, const Dictionary& options) {
    Array shapes;
    shapes.append(shape);
    Array empty_transforms;
    return export_assembly_with_options(shapes, empty_transforms, file_path, options);
}

bool ocgd_glb_exporter::export_assembly_with_options(const Array& shapes, const Array& transforms, const String& file_path, const Dictionary& options) {
    clear_error();
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");
    ERR_FAIL_COND_V_MSG(shapes.size() == 0, false, "No shapes provided for export");
    
    if (shapes.size() == 0) {
        last_error = "No shapes provided for export";
        ERR_PRINT(last_error);
        return false;
    }
    
    try {
        // Create XCAF document for GLB export
        Handle(TDocStd_Document) doc;
        Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
        app->NewDocument("MDTV-XCAF", doc);
        
        // Get tools
        Handle(XCAFDoc_ShapeTool) shape_tool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
        Handle(XCAFDoc_ColorTool) color_tool = XCAFDoc_DocumentTool::ColorTool(doc->Main());
        Handle(XCAFDoc_MaterialTool) material_tool = XCAFDoc_DocumentTool::MaterialTool(doc->Main());
        
        // Process export options
        double export_deflection = mesh_deflection;
        double export_angular = angular_deflection;
        if (options.has("mesh_deflection")) {
            export_deflection = options["mesh_deflection"];
        }
        if (options.has("angular_deflection")) {
            export_angular = options["angular_deflection"];
        }
        
        // Add shapes to document
        for (int i = 0; i < shapes.size(); i++) {
            Ref<ocgd_shape> shape_ref = shapes[i];
            if (shape_ref.is_null()) {
                last_error = String("Shape at index ") + String::num(i) + " is null";
                ERR_PRINT(last_error);
                return false;
            }
            if (!shape_ref->has_shape()) {
                last_error = String("Shape at index ") + String::num(i) + " has no valid OpenCASCADE shape";
                ERR_PRINT(last_error);
                return false;
            }
            
            TopoDS_Shape occ_shape = shape_ref->get_shape();
            
            // Mesh the shape before export
            BRepMesh_IncrementalMesh mesher(occ_shape, export_deflection, Standard_False, export_angular);
            
            // Add shape to document
            TDF_Label shape_label = shape_tool->AddShape(occ_shape);
            
            // Apply transform if provided
            if (i < transforms.size() && transforms[i].get_type() != Variant::NIL) {
                // Transform would be applied here - for now skip
                // This would require implementing transform parsing from Godot data
            }
            
            // Add default color if export_colors is enabled
            if (export_colors) {
                Quantity_Color default_color(0.7, 0.7, 0.7, Quantity_TOC_RGB);
                color_tool->SetColor(shape_label, default_color, XCAFDoc_ColorSurf);
            }
            
            // Add default material if export_materials is enabled
            if (export_materials) {
                Handle(TCollection_HAsciiString) mat_name = new TCollection_HAsciiString(default_material_name.utf8().get_data());
                TDF_Label material_label = material_tool->AddMaterial(mat_name, Handle(TCollection_HAsciiString)(), 1.0, Handle(TCollection_HAsciiString)(), Handle(TCollection_HAsciiString)());
                material_tool->SetMaterial(shape_label, material_label);
            }
        }
        
        // Create GLB writer
        RWGltf_CafWriter writer(TCollection_AsciiString(file_path.utf8().get_data()), Standard_True);
        
        // Configure writer options
        writer.SetMeshNameFormat(RWMesh_NameFormat_ProductOrInstance);
        writer.SetNodeNameFormat(RWMesh_NameFormat_ProductOrInstance);
        
        // Set coordinate system conversion (GLB uses Y-up, right-handed)
        RWMesh_CoordinateSystemConverter converter;
        converter.SetInputCoordinateSystem(RWMesh_CoordinateSystem_Zup);
        converter.SetOutputCoordinateSystem(RWMesh_CoordinateSystem_Yup);
        writer.SetCoordinateSystemConverter(converter);
        
        // Perform the export
        bool result = writer.Perform(doc, TColStd_IndexedDataMapOfStringString(), Message_ProgressRange());
        
        if (!result) {
            last_error = "GLB export failed during writing";
            ERR_PRINT(last_error);
            return false;
        }
        
        return true;
        
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error during GLB export: ") + e.GetMessageString();
        ERR_PRINT(last_error);
        return false;
    } catch (const std::exception& e) {
        last_error = String("Standard exception during GLB export: ") + e.what();
        ERR_PRINT(last_error);
        return false;
    } catch (...) {
        last_error = "Unknown exception during GLB export";
        ERR_PRINT(last_error);
        return false;
    }
}

void ocgd_glb_exporter::set_mesh_deflection(double deflection) {
    ERR_FAIL_COND_MSG(deflection <= 0.0, "Mesh deflection must be positive");
    mesh_deflection = deflection;
}

double ocgd_glb_exporter::get_mesh_deflection() const {
    return mesh_deflection;
}

void ocgd_glb_exporter::set_angular_deflection(double deflection) {
    ERR_FAIL_COND_MSG(deflection <= 0.0, "Angular deflection must be positive");
    angular_deflection = deflection;
}

double ocgd_glb_exporter::get_angular_deflection() const {
    return angular_deflection;
}

void ocgd_glb_exporter::set_export_colors(bool enable) {
    export_colors = enable;
}

bool ocgd_glb_exporter::get_export_colors() const {
    return export_colors;
}

void ocgd_glb_exporter::set_export_materials(bool enable) {
    export_materials = enable;
}

bool ocgd_glb_exporter::get_export_materials() const {
    return export_materials;
}

void ocgd_glb_exporter::set_export_textures(bool enable) {
    export_textures = enable;
}

bool ocgd_glb_exporter::get_export_textures() const {
    return export_textures;
}

void ocgd_glb_exporter::set_merge_faces(bool enable) {
    merge_faces = enable;
}

bool ocgd_glb_exporter::get_merge_faces() const {
    return merge_faces;
}

void ocgd_glb_exporter::set_texture_size(int size) {
    ERR_FAIL_COND_MSG(size <= 0, "Texture size must be positive");
    texture_size = size;
}

int ocgd_glb_exporter::get_texture_size() const {
    return texture_size;
}

void ocgd_glb_exporter::set_default_material_name(const String& name) {
    default_material_name = name;
}

String ocgd_glb_exporter::get_default_material_name() const {
    return default_material_name;
}

Dictionary ocgd_glb_exporter::get_export_info() const {
    Dictionary info;
    info["type"] = "GLB Exporter";
    info["mesh_deflection"] = mesh_deflection;
    info["angular_deflection"] = angular_deflection;
    info["export_colors"] = export_colors;
    info["export_materials"] = export_materials;
    info["export_textures"] = export_textures;
    info["merge_faces"] = merge_faces;
    info["texture_size"] = texture_size;
    info["default_material_name"] = default_material_name;
    info["last_error"] = last_error;
    return info;
}

Dictionary ocgd_glb_exporter::get_mesh_statistics() const {
    Dictionary stats;
    stats["triangle_count"] = get_exported_triangle_count();
    stats["vertex_count"] = get_exported_vertex_count();
    return stats;
}

int ocgd_glb_exporter::get_exported_triangle_count() const {
    // This would require storing mesh statistics during export
    return 0;
}

int ocgd_glb_exporter::get_exported_vertex_count() const {
    // This would require storing mesh statistics during export
    return 0;
}

void ocgd_glb_exporter::add_shape_color(const Ref<ocgd_shape>& shape, const Vector3& color) {
    ERR_FAIL_COND_MSG(shape.is_null(), "Shape is null");
    
    // Store the color mapping using the shape's memory address as key
    String shape_key = String::num_int64(reinterpret_cast<int64_t>(shape.ptr()));
    shape_colors[shape_key] = color;
}

void ocgd_glb_exporter::add_shape_material(const Ref<ocgd_shape>& shape, const Dictionary& material) {
    ERR_FAIL_COND_MSG(shape.is_null(), "Shape is null");
    
    // Store the material mapping using the shape's memory address as key
    String shape_key = String::num_int64(reinterpret_cast<int64_t>(shape.ptr()));
    shape_materials[shape_key] = material;
}

void ocgd_glb_exporter::clear_shape_colors() {
    shape_colors.clear();
}

void ocgd_glb_exporter::clear_shape_materials() {
    shape_materials.clear();
}

bool ocgd_glb_exporter::export_hierarchical_assembly(const Dictionary& assembly_tree, const String& file_path) {
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");
    ERR_FAIL_COND_V_MSG(assembly_tree.is_empty(), false, "Assembly tree is empty");
    
    try {
        // Extract shapes from assembly tree
        Array shapes;
        if (assembly_tree.has("shapes")) {
            shapes = assembly_tree["shapes"];
        }
        
        if (shapes.is_empty()) {
            last_error = "No shapes found in assembly tree";
            return false;
        }
        
        // For now, export as a single compound - full hierarchical support would need scene graph
        return export_shapes(shapes, file_path);
        
    } catch (const std::exception& e) {
        last_error = "Error during hierarchical assembly export: " + String(e.what());
        return false;
    } catch (...) {
        last_error = "Unknown error during hierarchical assembly export";
        return false;
    }
}

Dictionary ocgd_glb_exporter::create_assembly_node(const String& name, const Ref<ocgd_shape>& shape, const Array& transform) {
    Dictionary node;
    node["name"] = name;
    node["shape"] = shape;
    node["transform"] = transform;
    node["children"] = Array();
    return node;
}

void ocgd_glb_exporter::set_coordinate_system_conversion(const String& from_system, const String& to_system) {
    // Store coordinate system conversion settings
    // This would typically affect transformation matrices during export
    // Common conversions: "Z_UP" to "Y_UP", "RIGHT_HANDED" to "LEFT_HANDED"
    
    if (from_system == "Z_UP" && to_system == "Y_UP") {
        // GLB typically uses Y-up coordinate system
        // This setting would be used during mesh export to apply appropriate transforms
    }
}

void ocgd_glb_exporter::set_unit_scale(double scale) {
    ERR_FAIL_COND_MSG(scale <= 0.0, "Scale must be positive");
    
    // Store unit scale - this would be applied during mesh vertex generation
    // For example: CAD units (mm) to GLB units (m) would use scale = 0.001
    mesh_deflection *= scale; // Adjust mesh deflection accordingly
}

void ocgd_glb_exporter::set_y_up_conversion(bool enable) {
    // GLB format typically uses Y-up coordinate system
    // This setting would affect how vertex coordinates are transformed
    if (enable) {
        set_coordinate_system_conversion("Z_UP", "Y_UP");
    }
}

String ocgd_glb_exporter::get_last_error() const {
    return last_error;
}

bool ocgd_glb_exporter::has_error() const {
    return !last_error.is_empty();
}

void ocgd_glb_exporter::clear_error() {
    last_error = "";
}

bool ocgd_glb_exporter::validate_shapes_for_export(const Array& shapes) const {
    ERR_FAIL_COND_V_MSG(shapes.size() == 0, false, "No shapes provided for validation");
    
    try {
        for (int i = 0; i < shapes.size(); i++) {
            Ref<ocgd_shape> shape_ref = shapes[i];
            ERR_FAIL_COND_V_MSG(shape_ref.is_null(), false, 
                String("Shape at index ") + String::num(i) + " is null");
            ERR_FAIL_COND_V_MSG(!shape_ref->has_shape(), false,
                String("Shape at index ") + String::num(i) + " has no valid OpenCASCADE shape");
            
            TopoDS_Shape occ_shape = shape_ref->get_shape();
            ERR_FAIL_COND_V_MSG(occ_shape.IsNull(), false,
                String("OpenCASCADE shape at index ") + String::num(i) + " is null");
        }
        return true;
    } catch (const Standard_Failure& e) {
        ERR_PRINT(String("OpenCASCADE error validating shapes: ") + e.GetMessageString());
        return false;
    } catch (const std::exception& e) {
        ERR_PRINT(String("Standard exception validating shapes: ") + e.what());
        return false;
    } catch (...) {
        ERR_PRINT("Unknown exception validating shapes");
        return false;
    }
}

Dictionary ocgd_glb_exporter::preview_export(const Array& shapes) {
    Dictionary preview;
    preview["shape_count"] = shapes.size();
    preview["valid_shapes"] = validate_shapes_for_export(shapes);
    
    int total_faces = 0;
    int total_edges = 0;
    int total_vertices = 0;
    
    for (int i = 0; i < shapes.size(); i++) {
        Ref<ocgd_shape> shape_ref = shapes[i];
        if (shape_ref.is_valid() && shape_ref->has_shape()) {
            total_faces += shape_ref->get_num_faces();
            total_edges += shape_ref->get_num_edges();
            total_vertices += shape_ref->get_num_vertices();
        }
    }
    
    preview["total_faces"] = total_faces;
    preview["total_edges"] = total_edges;
    preview["total_vertices"] = total_vertices;
    preview["estimated_triangles"] = total_faces * 2; // Rough estimate
    
    return preview;
}

Array ocgd_glb_exporter::get_supported_extensions() const {
    Array extensions;
    extensions.append("glb");
    extensions.append("gltf");
    return extensions;
}

Dictionary ocgd_glb_exporter::get_export_capabilities() const {
    Dictionary capabilities;
    capabilities["meshes"] = true;
    capabilities["materials"] = export_materials;
    capabilities["colors"] = export_colors;
    capabilities["textures"] = export_textures;
    capabilities["assemblies"] = true;
    capabilities["animations"] = false;
    capabilities["instances"] = true;
    return capabilities;
}

String ocgd_glb_exporter::get_type() const {
    return "ocgd_glb_exporter";
}

bool ocgd_glb_exporter::is_null() const {
    return false;
}

void ocgd_glb_exporter::set_mesh_optimization(bool enable) {
    mesh_optimization = enable;
}

bool ocgd_glb_exporter::get_mesh_optimization() const {
    return mesh_optimization;
}

void ocgd_glb_exporter::set_vertex_welding_tolerance(double tolerance) {
    ERR_FAIL_COND_MSG(tolerance < 0.0, "Tolerance must be non-negative");
    
    // Store vertex welding tolerance - used during mesh optimization
    // This would be applied when mesh_optimization is enabled
    if (mesh_optimization) {
        // Tolerance affects how close vertices need to be to be considered identical
        // Smaller values preserve more detail, larger values reduce vertex count
    }
}

double ocgd_glb_exporter::get_vertex_welding_tolerance() const {
    // Return vertex welding tolerance
    return 0.001;
}