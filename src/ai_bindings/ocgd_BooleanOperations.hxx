#ifndef _ocgd_BooleanOperations_HeaderFile
#define _ocgd_BooleanOperations_HeaderFile

/**
 * ocgd_BooleanOperations.hxx
 *
 * Godot GDExtension wrapper for OpenCASCADE Boolean operations.
 *
 * This class provides methods to perform Boolean operations on OpenCASCADE shapes including:
 * - Union (Fuse): Combine two shapes into one
 * - Intersection (Common): Find the common volume between shapes
 * - Difference (Cut): Remove one shape from another
 * - Section: Find the intersection curves/surfaces between shapes
 * - Split: Split one shape by another
 *
 * Boolean operations are fundamental CAD operations used for:
 * - Creating complex shapes from simple primitives
 * - Removing material (drilling holes, cutting)
 * - Finding intersections and overlaps
 * - Shape analysis and validation
 *
 * The class provides both simple two-shape operations and advanced multi-shape
 * operations with options for fuzzy tolerance, parallel processing, and
 * result validation.
 *
 * Original OCCT headers: <opencascade/BRepAlgoAPI_Fuse.hxx>, 
 *                       <opencascade/BRepAlgoAPI_Common.hxx>,
 *                       <opencascade/BRepAlgoAPI_Cut.hxx>
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/BRepAlgoAPI_Fuse.hxx>
#include <opencascade/BRepAlgoAPI_Common.hxx>
#include <opencascade/BRepAlgoAPI_Cut.hxx>
#include <opencascade/BRepAlgoAPI_Section.hxx>
#include <opencascade/BRepAlgoAPI_Splitter.hxx>
#include <opencascade/Message_ProgressRange.hxx>

#include "ocgd_TopoDS_Shape.hxx"

using namespace godot;

/**
 * ocgd_BooleanOperations
 *
 * Wrapper class to expose OpenCASCADE Boolean operations to Godot.
 *
 * This class provides comprehensive Boolean operation capabilities including
 * basic two-shape operations, multi-shape operations, and advanced options
 * for controlling operation parameters and result quality.
 */
class ocgd_BooleanOperations : public RefCounted {
    GDCLASS(ocgd_BooleanOperations, RefCounted);

protected:
    static void _bind_methods();

private:
    // Operation settings
    double _fuzzy_tolerance;
    bool _run_parallel;
    bool _check_inverted;
    bool _use_oriented_bbox;

public:
    //! Default constructor
    ocgd_BooleanOperations();

    //! Destructor
    virtual ~ocgd_BooleanOperations();

    // Configuration Methods

    //! Set fuzzy tolerance for Boolean operations
    //! @param tolerance fuzzy tolerance value (0 = exact, >0 = tolerant)
    void set_fuzzy_tolerance(double tolerance);

    //! Get the current fuzzy tolerance setting
    double get_fuzzy_tolerance() const;

    //! Set whether to run operations in parallel when possible
    void set_run_parallel(bool run_parallel);

    //! Get parallel execution setting
    bool get_run_parallel() const;

    //! Set whether to check for inverted solids
    void set_check_inverted(bool check_inverted);

    //! Get inverted solid checking setting
    bool get_check_inverted() const;

    //! Set whether to use oriented bounding boxes for optimization
    void set_use_oriented_bbox(bool use_obb);

    //! Get oriented bounding box usage setting
    bool get_use_oriented_bbox() const;

    // Basic Two-Shape Boolean Operations

    //! Boolean Union (Fuse) - Combine two shapes
    //! @param shape1 first shape
    //! @param shape2 second shape
    //! @return the unified shape, or null if operation failed
    Ref<ocgd_TopoDS_Shape> union_shapes(const Ref<ocgd_TopoDS_Shape>& shape1, 
                                        const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Boolean Intersection (Common) - Find common volume
    //! @param shape1 first shape  
    //! @param shape2 second shape
    //! @return the intersection shape, or null if operation failed
    Ref<ocgd_TopoDS_Shape> intersect_shapes(const Ref<ocgd_TopoDS_Shape>& shape1,
                                            const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Boolean Difference (Cut) - Subtract second shape from first
    //! @param shape1 shape to cut from
    //! @param shape2 cutting shape
    //! @return the result shape, or null if operation failed
    Ref<ocgd_TopoDS_Shape> subtract_shapes(const Ref<ocgd_TopoDS_Shape>& shape1,
                                           const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Boolean Section - Find intersection curves/surfaces
    //! @param shape1 first shape
    //! @param shape2 second shape
    //! @return the section shape (curves/surfaces), or null if operation failed
    Ref<ocgd_TopoDS_Shape> section_shapes(const Ref<ocgd_TopoDS_Shape>& shape1,
                                          const Ref<ocgd_TopoDS_Shape>& shape2);

    // Multi-Shape Boolean Operations

    //! Union multiple shapes into one
    //! @param shapes array of shapes to unite
    //! @return the unified shape, or null if operation failed
    Ref<ocgd_TopoDS_Shape> union_multiple(const Array& shapes);

    //! Intersect multiple shapes (common volume of all)
    //! @param shapes array of shapes to intersect
    //! @return the intersection shape, or null if operation failed
    Ref<ocgd_TopoDS_Shape> intersect_multiple(const Array& shapes);

    //! Split a shape by multiple tools
    //! @param shape the shape to split
    //! @param tools array of splitting shapes
    //! @return the split shape, or null if operation failed
    Ref<ocgd_TopoDS_Shape> split_shape(const Ref<ocgd_TopoDS_Shape>& shape, const Array& tools);

    // Advanced Operations with Validation

    //! Boolean Union with validation and error reporting
    //! @param shape1 first shape
    //! @param shape2 second shape
    //! @return Dictionary with "result", "success", "errors", "warnings"
    Dictionary union_with_validation(const Ref<ocgd_TopoDS_Shape>& shape1,
                                    const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Boolean Intersection with validation and error reporting
    //! @param shape1 first shape
    //! @param shape2 second shape  
    //! @return Dictionary with "result", "success", "errors", "warnings"
    Dictionary intersect_with_validation(const Ref<ocgd_TopoDS_Shape>& shape1,
                                        const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Boolean Difference with validation and error reporting
    //! @param shape1 shape to cut from
    //! @param shape2 cutting shape
    //! @return Dictionary with "result", "success", "errors", "warnings"
    Dictionary subtract_with_validation(const Ref<ocgd_TopoDS_Shape>& shape1,
                                       const Ref<ocgd_TopoDS_Shape>& shape2);

    // Convenience Methods

    //! Quick union operation with default settings
    //! @param shape1 first shape
    //! @param shape2 second shape
    //! @return the unified shape, or null if failed
    Ref<ocgd_TopoDS_Shape> quick_union(const Ref<ocgd_TopoDS_Shape>& shape1,
                                       const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Quick intersection operation with default settings
    //! @param shape1 first shape
    //! @param shape2 second shape
    //! @return the intersection shape, or null if failed
    Ref<ocgd_TopoDS_Shape> quick_intersect(const Ref<ocgd_TopoDS_Shape>& shape1,
                                           const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Quick subtraction operation with default settings
    //! @param shape1 shape to cut from
    //! @param shape2 cutting shape
    //! @return the result shape, or null if failed
    Ref<ocgd_TopoDS_Shape> quick_subtract(const Ref<ocgd_TopoDS_Shape>& shape1,
                                          const Ref<ocgd_TopoDS_Shape>& shape2);

    // Validation and Analysis

    //! Check if two shapes can be used for Boolean operations
    //! @param shape1 first shape
    //! @param shape2 second shape
    //! @return Dictionary with validation results
    Dictionary validate_shapes_for_boolean(const Ref<ocgd_TopoDS_Shape>& shape1,
                                           const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Check if shapes intersect (have common volume)
    //! @param shape1 first shape
    //! @param shape2 second shape
    //! @return true if shapes intersect
    bool shapes_intersect(const Ref<ocgd_TopoDS_Shape>& shape1,
                         const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Check if one shape is completely inside another
    //! @param inner_shape shape to test if inside
    //! @param outer_shape shape to test if contains
    //! @return true if inner_shape is inside outer_shape
    bool shape_contains(const Ref<ocgd_TopoDS_Shape>& outer_shape,
                       const Ref<ocgd_TopoDS_Shape>& inner_shape);

    // Error Handling and Diagnostics

    //! Get the last operation error message
    String get_last_error() const;

    //! Get the last operation warning messages
    Array get_last_warnings() const;

    //! Clear error and warning history
    void clear_messages();

    // Performance Analysis

    //! Get timing information for the last operation
    //! @return Dictionary with timing statistics
    Dictionary get_operation_timing() const;

    //! Estimate operation complexity
    //! @param shape1 first shape
    //! @param shape2 second shape
    //! @param operation_type type of operation to estimate
    //! @return Dictionary with complexity metrics
    Dictionary estimate_operation_complexity(const Ref<ocgd_TopoDS_Shape>& shape1,
                                            const Ref<ocgd_TopoDS_Shape>& shape2,
                                            const String& operation_type) const;

    // Operation type enumeration
    enum OperationType {
        OPERATION_UNION = 0,
        OPERATION_INTERSECT = 1,
        OPERATION_SUBTRACT = 2,
        OPERATION_SECTION = 3,
        OPERATION_SPLIT = 4
    };

private:
    // Internal state for error tracking
    String _last_error;
    Array _last_warnings;
    Dictionary _last_timing;

    //! Internal helper to perform Boolean operation with validation
    Dictionary perform_boolean_operation(const Ref<ocgd_TopoDS_Shape>& shape1,
                                        const Ref<ocgd_TopoDS_Shape>& shape2,
                                        OperationType operation);

    //! Internal helper to validate operation parameters
    bool validate_operation_inputs(const Ref<ocgd_TopoDS_Shape>& shape1,
                                  const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Internal helper to extract operation results
    Ref<ocgd_TopoDS_Shape> extract_operation_result(void* api_operation, const String& operation_name);

    //! Internal helper to collect operation messages
    void collect_operation_messages(void* api_operation);
};

VARIANT_ENUM_CAST(ocgd_BooleanOperations::OperationType);

#endif // _ocgd_BooleanOperations_HeaderFile