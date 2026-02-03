/**
 * ocgd_BooleanOperations.cpp
 *
 * Godot GDExtension wrapper implementation for OpenCASCADE Boolean operations.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_BooleanOperations.hxx"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <opencascade/BRepAlgoAPI_Fuse.hxx>
#include <opencascade/BRepAlgoAPI_Common.hxx>
#include <opencascade/BRepAlgoAPI_Cut.hxx>
#include <opencascade/BRepAlgoAPI_Section.hxx>
#include <opencascade/BRepAlgoAPI_Splitter.hxx>
#include <opencascade/BRepCheck_Analyzer.hxx>
#include <opencascade/Message_ProgressRange.hxx>
#include <opencascade/Standard_Failure.hxx>

using namespace godot;

ocgd_BooleanOperations::ocgd_BooleanOperations() {
    _fuzzy_tolerance = 1e-6;
    _run_parallel = false;
    _check_inverted = true;
    _use_oriented_bbox = false;
    _last_error = "";
}

ocgd_BooleanOperations::~ocgd_BooleanOperations() {
    // No cleanup needed
}

void ocgd_BooleanOperations::set_fuzzy_tolerance(double tolerance) {
    _fuzzy_tolerance = tolerance;
}

double ocgd_BooleanOperations::get_fuzzy_tolerance() const {
    return _fuzzy_tolerance;
}

void ocgd_BooleanOperations::set_run_parallel(bool run_parallel) {
    _run_parallel = run_parallel;
}

bool ocgd_BooleanOperations::get_run_parallel() const {
    return _run_parallel;
}

void ocgd_BooleanOperations::set_check_inverted(bool check_inverted) {
    _check_inverted = check_inverted;
}

bool ocgd_BooleanOperations::get_check_inverted() const {
    return _check_inverted;
}

void ocgd_BooleanOperations::set_use_oriented_bbox(bool use_obb) {
    _use_oriented_bbox = use_obb;
}

bool ocgd_BooleanOperations::get_use_oriented_bbox() const {
    return _use_oriented_bbox;
}

Ref<ocgd_TopoDS_Shape> ocgd_BooleanOperations::union_shapes(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    if (!validate_operation_inputs(shape1, shape2)) {
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        const TopoDS_Shape& occt_shape1 = shape1->get_occt_shape();
        const TopoDS_Shape& occt_shape2 = shape2->get_occt_shape();
        
        BRepAlgoAPI_Fuse fuse_op(occt_shape1, occt_shape2);
        
        if (_fuzzy_tolerance > 0) {
            fuse_op.SetFuzzyValue(_fuzzy_tolerance);
        }
        
        fuse_op.SetRunParallel(_run_parallel);
        fuse_op.Build();
        
        if (fuse_op.IsDone() && !fuse_op.Shape().IsNull()) {
            Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
            result->set_occt_shape(fuse_op.Shape());
            _last_error = "";
            return result;
        } else {
            _last_error = "Union operation failed";
        }
    } catch (const Standard_Failure& e) {
        _last_error = String("Union operation failed: ") + String(e.GetMessageString());
        UtilityFunctions::printerr("BooleanOperations: Union failed with Standard_Failure - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        _last_error = String("Union operation failed: ") + String(e.what());
        UtilityFunctions::printerr("BooleanOperations: Union failed with exception - " + String(e.what()));
    }
    
    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_BooleanOperations::intersect_shapes(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    if (!validate_operation_inputs(shape1, shape2)) {
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        const TopoDS_Shape& occt_shape1 = shape1->get_occt_shape();
        const TopoDS_Shape& occt_shape2 = shape2->get_occt_shape();
        
        BRepAlgoAPI_Common common_op(occt_shape1, occt_shape2);
        
        if (_fuzzy_tolerance > 0) {
            common_op.SetFuzzyValue(_fuzzy_tolerance);
        }
        
        common_op.SetRunParallel(_run_parallel);
        common_op.Build();
        
        if (common_op.IsDone() && !common_op.Shape().IsNull()) {
            Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
            result->set_occt_shape(common_op.Shape());
            _last_error = "";
            return result;
        } else {
            _last_error = "Intersection operation failed";
        }
    } catch (const Standard_Failure& e) {
        _last_error = String("Intersection operation failed: ") + String(e.GetMessageString());
        UtilityFunctions::printerr("BooleanOperations: Intersection failed with Standard_Failure - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        _last_error = String("Intersection operation failed: ") + String(e.what());
        UtilityFunctions::printerr("BooleanOperations: Intersection failed with exception - " + String(e.what()));
    }
    
    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_BooleanOperations::subtract_shapes(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    if (!validate_operation_inputs(shape1, shape2)) {
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        const TopoDS_Shape& occt_shape1 = shape1->get_occt_shape();
        const TopoDS_Shape& occt_shape2 = shape2->get_occt_shape();
        
        BRepAlgoAPI_Cut cut_op(occt_shape1, occt_shape2);
        
        if (_fuzzy_tolerance > 0) {
            cut_op.SetFuzzyValue(_fuzzy_tolerance);
        }
        
        cut_op.SetRunParallel(_run_parallel);
        cut_op.Build();
        
        if (cut_op.IsDone() && !cut_op.Shape().IsNull()) {
            Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
            result->set_occt_shape(cut_op.Shape());
            _last_error = "";
            return result;
        } else {
            _last_error = "Subtraction operation failed";
        }
    } catch (const Standard_Failure& e) {
        _last_error = String("Subtraction operation failed: ") + String(e.GetMessageString());
        UtilityFunctions::printerr("BooleanOperations: Subtraction failed with Standard_Failure - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        _last_error = String("Subtraction operation failed: ") + String(e.what());
        UtilityFunctions::printerr("BooleanOperations: Subtraction failed with exception - " + String(e.what()));
    }
    
    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_BooleanOperations::section_shapes(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    if (!validate_operation_inputs(shape1, shape2)) {
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        const TopoDS_Shape& occt_shape1 = shape1->get_occt_shape();
        const TopoDS_Shape& occt_shape2 = shape2->get_occt_shape();
        
        BRepAlgoAPI_Section section_op(occt_shape1, occt_shape2);
        
        if (_fuzzy_tolerance > 0) {
            section_op.SetFuzzyValue(_fuzzy_tolerance);
        }
        
        section_op.Build();
        
        if (section_op.IsDone() && !section_op.Shape().IsNull()) {
            Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
            result->set_occt_shape(section_op.Shape());
            _last_error = "";
            return result;
        } else {
            _last_error = "Section operation failed";
        }
    } catch (const Standard_Failure& e) {
        _last_error = String("Section operation failed: ") + String(e.GetMessageString());
        UtilityFunctions::printerr("BooleanOperations: Section failed with Standard_Failure - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        _last_error = String("Section operation failed: ") + String(e.what());
        UtilityFunctions::printerr("BooleanOperations: Section failed with exception - " + String(e.what()));
    }
    
    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_BooleanOperations::union_multiple(const Array& shapes) {
    if (shapes.size() < 2) {
        _last_error = "Need at least 2 shapes for union operation";
        return Ref<ocgd_TopoDS_Shape>();
    }

    Ref<ocgd_TopoDS_Shape> result = shapes[0];
    
    for (int i = 1; i < shapes.size(); i++) {
        Ref<ocgd_TopoDS_Shape> next_shape = shapes[i];
        result = union_shapes(result, next_shape);
        
        if (result.is_null()) {
            _last_error = "Multiple union failed at shape " + String::num(i);
            return Ref<ocgd_TopoDS_Shape>();
        }
    }
    
    return result;
}

Ref<ocgd_TopoDS_Shape> ocgd_BooleanOperations::intersect_multiple(const Array& shapes) {
    if (shapes.size() < 2) {
        _last_error = "Need at least 2 shapes for intersection operation";
        return Ref<ocgd_TopoDS_Shape>();
    }

    Ref<ocgd_TopoDS_Shape> result = shapes[0];
    
    for (int i = 1; i < shapes.size(); i++) {
        Ref<ocgd_TopoDS_Shape> next_shape = shapes[i];
        result = intersect_shapes(result, next_shape);
        
        if (result.is_null()) {
            _last_error = "Multiple intersection failed at shape " + String::num(i);
            return Ref<ocgd_TopoDS_Shape>();
        }
    }
    
    return result;
}

Dictionary ocgd_BooleanOperations::union_with_validation(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    Dictionary result;
    
    // Validate inputs
    Dictionary validation = validate_shapes_for_boolean(shape1, shape2);
    result["input_validation"] = validation;
    
    if (!validation.get("valid", false)) {
        result["success"] = false;
        result["error"] = "Input validation failed";
        return result;
    }
    
    // Perform operation
    Ref<ocgd_TopoDS_Shape> union_result = union_shapes(shape1, shape2);
    
    if (union_result.is_null()) {
        result["success"] = false;
        result["error"] = _last_error;
        return result;
    }
    
    // Validate result
    BRepCheck_Analyzer analyzer(union_result->get_occt_shape());
    result["success"] = true;
    result["result"] = union_result;
    result["result_valid"] = analyzer.IsValid() == Standard_True;
    result["errors"] = Array();
    result["warnings"] = Array();
    
    return result;
}

Dictionary ocgd_BooleanOperations::intersect_with_validation(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    Dictionary result;
    
    Dictionary validation = validate_shapes_for_boolean(shape1, shape2);
    result["input_validation"] = validation;
    
    if (!validation.get("valid", false)) {
        result["success"] = false;
        result["error"] = "Input validation failed";
        return result;
    }
    
    Ref<ocgd_TopoDS_Shape> intersect_result = intersect_shapes(shape1, shape2);
    
    if (intersect_result.is_null()) {
        result["success"] = false;
        result["error"] = _last_error;
        return result;
    }
    
    BRepCheck_Analyzer analyzer(intersect_result->get_occt_shape());
    result["success"] = true;
    result["result"] = intersect_result;
    result["result_valid"] = analyzer.IsValid() == Standard_True;
    result["errors"] = Array();
    result["warnings"] = Array();
    
    return result;
}

Dictionary ocgd_BooleanOperations::subtract_with_validation(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    Dictionary result;
    
    Dictionary validation = validate_shapes_for_boolean(shape1, shape2);
    result["input_validation"] = validation;
    
    if (!validation.get("valid", false)) {
        result["success"] = false;
        result["error"] = "Input validation failed";
        return result;
    }
    
    Ref<ocgd_TopoDS_Shape> subtract_result = subtract_shapes(shape1, shape2);
    
    if (subtract_result.is_null()) {
        result["success"] = false;
        result["error"] = _last_error;
        return result;
    }
    
    BRepCheck_Analyzer analyzer(subtract_result->get_occt_shape());
    result["success"] = true;
    result["result"] = subtract_result;
    result["result_valid"] = analyzer.IsValid() == Standard_True;
    result["errors"] = Array();
    result["warnings"] = Array();
    
    return result;
}

Ref<ocgd_TopoDS_Shape> ocgd_BooleanOperations::quick_union(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    return union_shapes(shape1, shape2);
}

Ref<ocgd_TopoDS_Shape> ocgd_BooleanOperations::quick_intersect(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    return intersect_shapes(shape1, shape2);
}

Ref<ocgd_TopoDS_Shape> ocgd_BooleanOperations::quick_subtract(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    return subtract_shapes(shape1, shape2);
}

Dictionary ocgd_BooleanOperations::validate_shapes_for_boolean(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    Dictionary result;
    Array errors;
    Array warnings;
    
    if (shape1.is_null() || shape1->is_null()) {
        errors.append("Shape 1 is null");
    }
    
    if (shape2.is_null() || shape2->is_null()) {
        errors.append("Shape 2 is null");
    }
    
    if (errors.size() > 0) {
        result["valid"] = false;
        result["errors"] = errors;
        result["warnings"] = warnings;
        return result;
    }
    
    // Check shape validity
    BRepCheck_Analyzer analyzer1(shape1->get_occt_shape());
    if (analyzer1.IsValid() != Standard_True) {
        errors.append("Shape 1 has geometric errors");
    }
    
    BRepCheck_Analyzer analyzer2(shape2->get_occt_shape());
    if (analyzer2.IsValid() != Standard_True) {
        errors.append("Shape 2 has geometric errors");
    }
    
    result["valid"] = errors.size() == 0;
    result["errors"] = errors;
    result["warnings"] = warnings;
    
    return result;
}

String ocgd_BooleanOperations::get_last_error() const {
    return _last_error;
}

Array ocgd_BooleanOperations::get_last_warnings() const {
    return _last_warnings;
}

void ocgd_BooleanOperations::clear_messages() {
    _last_error = "";
    _last_warnings.clear();
}

bool ocgd_BooleanOperations::validate_operation_inputs(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    if (shape1.is_null() || shape1->is_null()) {
        _last_error = "First shape is null";
        UtilityFunctions::printerr("BooleanOperations: First shape is null");
        return false;
    }
    
    if (shape2.is_null() || shape2->is_null()) {
        _last_error = "Second shape is null";
        UtilityFunctions::printerr("BooleanOperations: Second shape is null");
        return false;
    }
    
    return true;
}

void ocgd_BooleanOperations::_bind_methods() {
    // Configuration properties
    ClassDB::bind_method(D_METHOD("set_fuzzy_tolerance", "tolerance"), &ocgd_BooleanOperations::set_fuzzy_tolerance);
    ClassDB::bind_method(D_METHOD("get_fuzzy_tolerance"), &ocgd_BooleanOperations::get_fuzzy_tolerance);
    ClassDB::add_property("ocgd_BooleanOperations", PropertyInfo(Variant::FLOAT, "fuzzy_tolerance"), "set_fuzzy_tolerance", "get_fuzzy_tolerance");
    
    ClassDB::bind_method(D_METHOD("set_run_parallel", "run_parallel"), &ocgd_BooleanOperations::set_run_parallel);
    ClassDB::bind_method(D_METHOD("get_run_parallel"), &ocgd_BooleanOperations::get_run_parallel);
    ClassDB::add_property("ocgd_BooleanOperations", PropertyInfo(Variant::BOOL, "run_parallel"), "set_run_parallel", "get_run_parallel");
    
    ClassDB::bind_method(D_METHOD("set_check_inverted", "check_inverted"), &ocgd_BooleanOperations::set_check_inverted);
    ClassDB::bind_method(D_METHOD("get_check_inverted"), &ocgd_BooleanOperations::get_check_inverted);
    ClassDB::add_property("ocgd_BooleanOperations", PropertyInfo(Variant::BOOL, "check_inverted"), "set_check_inverted", "get_check_inverted");
    
    ClassDB::bind_method(D_METHOD("set_use_oriented_bbox", "use_obb"), &ocgd_BooleanOperations::set_use_oriented_bbox);
    ClassDB::bind_method(D_METHOD("get_use_oriented_bbox"), &ocgd_BooleanOperations::get_use_oriented_bbox);
    ClassDB::add_property("ocgd_BooleanOperations", PropertyInfo(Variant::BOOL, "use_oriented_bbox"), "set_use_oriented_bbox", "get_use_oriented_bbox");
    
    // Basic Boolean operations
    ClassDB::bind_method(D_METHOD("union_shapes", "shape1", "shape2"), &ocgd_BooleanOperations::union_shapes);
    ClassDB::bind_method(D_METHOD("intersect_shapes", "shape1", "shape2"), &ocgd_BooleanOperations::intersect_shapes);
    ClassDB::bind_method(D_METHOD("subtract_shapes", "shape1", "shape2"), &ocgd_BooleanOperations::subtract_shapes);
    ClassDB::bind_method(D_METHOD("section_shapes", "shape1", "shape2"), &ocgd_BooleanOperations::section_shapes);
    
    // Multi-shape operations
    ClassDB::bind_method(D_METHOD("union_multiple", "shapes"), &ocgd_BooleanOperations::union_multiple);
    ClassDB::bind_method(D_METHOD("intersect_multiple", "shapes"), &ocgd_BooleanOperations::intersect_multiple);
    
    // Operations with validation
    ClassDB::bind_method(D_METHOD("union_with_validation", "shape1", "shape2"), &ocgd_BooleanOperations::union_with_validation);
    ClassDB::bind_method(D_METHOD("intersect_with_validation", "shape1", "shape2"), &ocgd_BooleanOperations::intersect_with_validation);
    ClassDB::bind_method(D_METHOD("subtract_with_validation", "shape1", "shape2"), &ocgd_BooleanOperations::subtract_with_validation);
    
    // Quick operations
    ClassDB::bind_method(D_METHOD("quick_union", "shape1", "shape2"), &ocgd_BooleanOperations::quick_union);
    ClassDB::bind_method(D_METHOD("quick_intersect", "shape1", "shape2"), &ocgd_BooleanOperations::quick_intersect);
    ClassDB::bind_method(D_METHOD("quick_subtract", "shape1", "shape2"), &ocgd_BooleanOperations::quick_subtract);
    
    // Validation methods
    ClassDB::bind_method(D_METHOD("validate_shapes_for_boolean", "shape1", "shape2"), &ocgd_BooleanOperations::validate_shapes_for_boolean);
    
    // Error handling
    ClassDB::bind_method(D_METHOD("get_last_error"), &ocgd_BooleanOperations::get_last_error);
    ClassDB::bind_method(D_METHOD("get_last_warnings"), &ocgd_BooleanOperations::get_last_warnings);
    ClassDB::bind_method(D_METHOD("clear_messages"), &ocgd_BooleanOperations::clear_messages);
    
    // Operation type enum constants
    BIND_ENUM_CONSTANT(OPERATION_UNION);
    BIND_ENUM_CONSTANT(OPERATION_INTERSECT);
    BIND_ENUM_CONSTANT(OPERATION_SUBTRACT);
    BIND_ENUM_CONSTANT(OPERATION_SECTION);
    BIND_ENUM_CONSTANT(OPERATION_SPLIT);
}