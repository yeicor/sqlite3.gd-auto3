/**
 * ocgd_TopologyAnalyzer.hxx
 *
 * Godot GDExtension wrapper header for OpenCASCADE comprehensive topology analysis.
 *
 * This class provides advanced topology analysis capabilities including:
 * - Detailed shape hierarchy exploration
 * - Geometric property analysis (areas, volumes, lengths)
 * - Connectivity analysis between topology elements
 * - Shape classification and validation
 * - Advanced measurement tools (distances, angles, curvature)
 * - Cross-sectional analysis
 * - Feature recognition and classification
 * - Quality assessment and mesh analysis
 *
 * Features:
 * - Complete topology traversal with detailed statistics
 * - Multi-level shape analysis (solid, shell, face, wire, edge, vertex)
 * - Geometric calculations with high precision
 * - Spatial relationship analysis between elements
 * - Feature detection (holes, fillets, chamfers, etc.)
 * - Shape comparison and similarity analysis
 * - Manufacturing analysis (undercuts, draft angles, etc.)
 * - Export capabilities for analysis results
 *
 * Original OCCT headers: <opencascade/TopExp_Explorer.hxx>,
 *                       <opencascade/BRepGProp.hxx>,
 *                       <opencascade/BRepExtrema_DistShapeShape.hxx>,
 *                       <opencascade/GeomLProp_SLProps.hxx>
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#ifndef OCGD_TOPOLOGY_ANALYZER_HXX
#define OCGD_TOPOLOGY_ANALYZER_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/TopAbs_ShapeEnum.hxx>
#include <opencascade/TopAbs_Orientation.hxx>
#include <opencascade/BRepGProp.hxx>
#include <opencascade/GProp_GProps.hxx>
#include <opencascade/BRepExtrema_DistShapeShape.hxx>
#include <opencascade/BRepClass3d_SolidClassifier.hxx>
#include <opencascade/BRepCheck_Analyzer.hxx>
#include <opencascade/BRepBuilderAPI_Sewing.hxx>
#include <opencascade/GeomLProp_SLProps.hxx>
#include <opencascade/BRepLProp_SLProps.hxx>
#include <opencascade/BRepAdaptor_Surface.hxx>
#include <opencascade/BRepAdaptor_Curve.hxx>
#include <opencascade/GeomAbs_SurfaceType.hxx>
#include <opencascade/GeomAbs_CurveType.hxx>
#include <opencascade/Bnd_Box.hxx>
#include <opencascade/BRepBndLib.hxx>
#include <opencascade/BRep_Tool.hxx>
#include <opencascade/TopoDS_Face.hxx>
#include <opencascade/TopoDS_Edge.hxx>
#include <opencascade/TopoDS_Vertex.hxx>
#include <opencascade/TopoDS_Wire.hxx>
#include <opencascade/TopoDS_Shell.hxx>
#include <opencascade/TopoDS_Solid.hxx>
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/gp_Vec.hxx>
#include <opencascade/gp_Dir.hxx>
#include <opencascade/gp_Ax1.hxx>
#include <opencascade/gp_Pln.hxx>

#include "ocgd_TopoDS_Shape.hxx"

using namespace godot;

/**
 * @brief Comprehensive topology analyzer for OpenCASCADE shapes
 * 
 * This class provides extensive topology analysis capabilities for detailed
 * exploration and measurement of CAD shapes, including geometric properties,
 * connectivity analysis, and feature recognition.
 */
class ocgd_TopologyAnalyzer : public RefCounted {
    GDCLASS(ocgd_TopologyAnalyzer, RefCounted)

public:
    /**
     * @brief Analysis depth levels
     */
    enum AnalysisDepth {
        DEPTH_BASIC = 0,        ///< Basic shape info and counts
        DEPTH_GEOMETRIC = 1,    ///< Include geometric properties
        DEPTH_CONNECTIVITY = 2, ///< Include connectivity analysis
        DEPTH_DETAILED = 3,     ///< Include detailed feature analysis
        DEPTH_COMPLETE = 4      ///< Complete analysis with all features
    };

    /**
     * @brief Shape classification types
     */
    enum ShapeClass {
        CLASS_UNKNOWN = 0,
        CLASS_SIMPLE_SOLID = 1,     ///< Single solid with no holes
        CLASS_HOLLOW_SOLID = 2,     ///< Solid with internal cavities
        CLASS_SHEET_BODY = 3,       ///< Open surface/shell
        CLASS_WIREFRAME = 4,        ///< Wire/edge only
        CLASS_POINT_CLOUD = 5,      ///< Vertices only
        CLASS_ASSEMBLY = 6,         ///< Multiple disconnected solids
        CLASS_SURFACE = 7,          ///< Single surface
        CLASS_CURVE = 8            ///< Single curve
    };

    /**
     * @brief Surface classification types
     */
    enum SurfaceClass {
        SURFACE_PLANE = 0,
        SURFACE_CYLINDER = 1,
        SURFACE_CONE = 2,
        SURFACE_SPHERE = 3,
        SURFACE_TORUS = 4,
        SURFACE_BEZIER = 5,
        SURFACE_BSPLINE = 6,
        SURFACE_REVOLUTION = 7,
        SURFACE_EXTRUSION = 8,
        SURFACE_OTHER = 9
    };

    /**
     * @brief Curve classification types
     */
    enum CurveClass {
        CURVE_LINE = 0,
        CURVE_CIRCLE = 1,
        CURVE_ELLIPSE = 2,
        CURVE_HYPERBOLA = 3,
        CURVE_PARABOLA = 4,
        CURVE_BEZIER = 5,
        CURVE_BSPLINE = 6,
        CURVE_OTHER = 7
    };

private:
    // Analysis settings
    AnalysisDepth _analysis_depth;
    double _tolerance;
    bool _include_mass_properties;
    bool _include_bounding_info;
    bool _include_connectivity;
    bool _include_curvature;
    bool _validate_geometry;
    
    // Caching
    mutable Dictionary _cached_analysis;
    mutable bool _cache_valid;
    mutable Ref<ocgd_TopoDS_Shape> _cached_shape;
    
    // Error handling
    mutable String _last_error;

protected:
    static void _bind_methods();

public:
    ocgd_TopologyAnalyzer();
    virtual ~ocgd_TopologyAnalyzer();

    // === Configuration Methods ===
    
    /**
     * @brief Set analysis depth level
     */
    void set_analysis_depth(AnalysisDepth depth);
    AnalysisDepth get_analysis_depth() const;

    /**
     * @brief Set tolerance for geometric calculations
     */
    void set_tolerance(double tolerance);
    double get_tolerance() const;

    /**
     * @brief Enable/disable mass properties calculation
     */
    void set_include_mass_properties(bool enabled);
    bool get_include_mass_properties() const;

    /**
     * @brief Enable/disable bounding box calculation
     */
    void set_include_bounding_info(bool enabled);
    bool get_include_bounding_info() const;

    /**
     * @brief Enable/disable connectivity analysis
     */
    void set_include_connectivity(bool enabled);
    bool get_include_connectivity() const;

    /**
     * @brief Enable/disable curvature analysis
     */
    void set_include_curvature(bool enabled);
    bool get_include_curvature() const;

    /**
     * @brief Enable/disable geometry validation
     */
    void set_validate_geometry(bool enabled);
    bool get_validate_geometry() const;

    // === Main Analysis Methods ===

    /**
     * @brief Perform comprehensive topology analysis
     */
    Dictionary analyze_shape(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Get detailed topology counts and hierarchy
     */
    Dictionary get_topology_summary(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Analyze geometric properties (areas, volumes, centroids)
     */
    Dictionary analyze_geometric_properties(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Analyze mass properties (inertia, center of mass)
     */
    Dictionary analyze_mass_properties(const Ref<ocgd_TopoDS_Shape>& shape, double density = 1.0);

    /**
     * @brief Get bounding box information
     */
    Dictionary get_bounding_info(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Classify the overall shape type
     */
    ShapeClass classify_shape(const Ref<ocgd_TopoDS_Shape>& shape);

    // === Element-Specific Analysis ===

    /**
     * @brief Analyze all faces in the shape
     */
    Array analyze_faces(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Analyze all edges in the shape
     */
    Array analyze_edges(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Analyze all vertices in the shape
     */
    Array analyze_vertices(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Get detailed face analysis for specific face
     */
    Dictionary analyze_face_detailed(const Ref<ocgd_TopoDS_Shape>& shape, int face_index);

    /**
     * @brief Get detailed edge analysis for specific edge
     */
    Dictionary analyze_edge_detailed(const Ref<ocgd_TopoDS_Shape>& shape, int edge_index);

    // === Connectivity Analysis ===

    /**
     * @brief Analyze connectivity between topology elements
     */
    Dictionary analyze_connectivity(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Find adjacent faces for each edge
     */
    Dictionary get_face_adjacency(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Find shared edges between faces
     */
    Dictionary get_edge_sharing(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Detect boundaries and free edges
     */
    Dictionary find_boundaries(const Ref<ocgd_TopoDS_Shape>& shape);

    // === Measurement Tools ===

    /**
     * @brief Calculate distance between two shapes
     */
    Dictionary calculate_distance(const Ref<ocgd_TopoDS_Shape>& shape1, 
                                 const Ref<ocgd_TopoDS_Shape>& shape2);

    /**
     * @brief Calculate distance from point to shape
     */
    Dictionary distance_point_to_shape(const Vector3& point, 
                                      const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Find closest points between two shapes
     */
    Array find_closest_points(const Ref<ocgd_TopoDS_Shape>& shape1, 
                             const Ref<ocgd_TopoDS_Shape>& shape2);

    /**
     * @brief Calculate angle between two faces at shared edge
     */
    double calculate_face_angle(const Ref<ocgd_TopoDS_Shape>& shape, 
                               int face1_index, int face2_index);

    // === Curvature Analysis ===

    /**
     * @brief Analyze curvature properties of faces
     */
    Array analyze_face_curvature(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Analyze curvature properties of edges
     */
    Array analyze_edge_curvature(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Get curvature at specific point on face
     */
    Dictionary get_face_curvature_at_point(const Ref<ocgd_TopoDS_Shape>& shape, 
                                          int face_index, const Vector3& point);

    // === Feature Detection ===

    /**
     * @brief Detect geometric features (fillets, chamfers, holes)
     */
    Dictionary detect_features(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Find cylindrical holes
     */
    Array find_cylindrical_holes(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Find fillet edges
     */
    Array find_fillets(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Find chamfer edges
     */
    Array find_chamfers(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Detect planar faces and group them
     */
    Dictionary group_planar_faces(const Ref<ocgd_TopoDS_Shape>& shape);

    // === Quality Analysis ===

    /**
     * @brief Validate shape geometry and topology
     */
    Dictionary validate_shape(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Analyze mesh quality if triangulation exists
     */
    Dictionary analyze_mesh_quality(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Check for common geometry issues
     */
    Dictionary check_geometry_issues(const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Calculate shape complexity metrics
     */
    Dictionary calculate_complexity_metrics(const Ref<ocgd_TopoDS_Shape>& shape);

    // === Spatial Analysis ===

    /**
     * @brief Check if point is inside solid
     */
    bool is_point_inside(const Vector3& point, const Ref<ocgd_TopoDS_Shape>& shape);

    /**
     * @brief Find intersection between two shapes
     */
    Dictionary find_intersection(const Ref<ocgd_TopoDS_Shape>& shape1, 
                                const Ref<ocgd_TopoDS_Shape>& shape2);

    /**
     * @brief Create cross-section at specified plane
     */
    Array create_cross_section(const Ref<ocgd_TopoDS_Shape>& shape, 
                              const Vector3& plane_origin, const Vector3& plane_normal);

    // === Comparison Tools ===

    /**
     * @brief Compare two shapes for similarity
     */
    Dictionary compare_shapes(const Ref<ocgd_TopoDS_Shape>& shape1, 
                             const Ref<ocgd_TopoDS_Shape>& shape2);

    /**
     * @brief Calculate shape signature for comparison
     */
    Dictionary calculate_shape_signature(const Ref<ocgd_TopoDS_Shape>& shape);

    // === Utility Methods ===

    /**
     * @brief Clear analysis cache
     */
    void clear_cache();

    /**
     * @brief Get last error message
     */
    String get_last_error() const;

    /**
     * @brief Export analysis results to file
     */
    bool export_analysis_report(const Dictionary& analysis, const String& file_path);

private:
    // === Internal Helper Methods ===
    
    /**
     * @brief Count topology elements by type
     */
    Dictionary count_topology_elements(const TopoDS_Shape& shape) const;

    /**
     * @brief Classify surface type
     */
    SurfaceClass classify_surface(const TopoDS_Face& face) const;

    /**
     * @brief Classify curve type
     */
    CurveClass classify_curve(const TopoDS_Edge& edge) const;

    /**
     * @brief Calculate face area and properties
     */
    Dictionary analyze_face_properties(const TopoDS_Face& face) const;

    /**
     * @brief Calculate edge length and properties
     */
    Dictionary analyze_edge_properties(const TopoDS_Edge& edge) const;

    /**
     * @brief Find face neighbors for connectivity analysis
     */
    Array find_face_neighbors(const TopoDS_Shape& shape, const TopoDS_Face& face) const;

    /**
     * @brief Calculate local curvature at surface point
     */
    Dictionary calculate_surface_curvature(const TopoDS_Face& face, 
                                          const gp_Pnt& point) const;

    /**
     * @brief Detect feature patterns
     */
    bool is_fillet_edge(const TopoDS_Edge& edge, const TopoDS_Shape& shape) const;
    bool is_chamfer_edge(const TopoDS_Edge& edge, const TopoDS_Shape& shape) const;
    bool is_hole_face(const TopoDS_Face& face, const TopoDS_Shape& shape) const;

    /**
     * @brief Set error message
     */
    void set_error(const String& error) const;

    /**
     * @brief Convert OCCT point to Godot Vector3
     */
    Vector3 gp_pnt_to_vector3(const gp_Pnt& point) const;

    /**
     * @brief Convert OCCT direction to Godot Vector3
     */
    Vector3 gp_dir_to_vector3(const gp_Dir& direction) const;

    /**
     * @brief Convert shape enum to string
     */
    String shape_type_to_string(TopAbs_ShapeEnum type) const;

    /**
     * @brief Convert orientation enum to string
     */
    String orientation_to_string(TopAbs_Orientation orientation) const;
};

VARIANT_ENUM_CAST(ocgd_TopologyAnalyzer::AnalysisDepth);
VARIANT_ENUM_CAST(ocgd_TopologyAnalyzer::ShapeClass);
VARIANT_ENUM_CAST(ocgd_TopologyAnalyzer::SurfaceClass);
VARIANT_ENUM_CAST(ocgd_TopologyAnalyzer::CurveClass);

#endif // OCGD_TOPOLOGY_ANALYZER_HXX