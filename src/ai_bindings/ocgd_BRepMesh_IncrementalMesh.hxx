#ifndef _ocgd_BRepMesh_IncrementalMesh_HeaderFile
#define _ocgd_BRepMesh_IncrementalMesh_HeaderFile

/**
 * ocgd_BRepMesh_IncrementalMesh.hxx
 *
 * Godot GDExtension wrapper for OpenCASCADE BRepMesh_IncrementalMesh class.
 *
 * Builds the mesh of a shape with respect of their correctly triangulated parts.
 * This class provides incremental meshing functionality for OpenCASCADE shapes,
 * allowing control over mesh quality and parameters.
 *
 * Original OCCT header: <opencascade/BRepMesh_IncrementalMesh.hxx>
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

#include <opencascade/BRepMesh_IncrementalMesh.hxx>
#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/IMeshTools_Parameters.hxx>
#include <opencascade/Message_ProgressRange.hxx>
#include <opencascade/Standard_Handle.hxx>

#include "ocgd_TopoDS_Shape.hxx"

using namespace godot;

/**
 * ocgd_BRepMesh_IncrementalMesh
 *
 * Wrapper class to expose the OpenCASCADE BRepMesh_IncrementalMesh to Godot.
 *
 * Builds the mesh of a shape with respect of their correctly triangulated parts.
 * This class provides incremental meshing functionality for OpenCASCADE shapes,
 * allowing control over mesh quality and parameters.
 */
class ocgd_BRepMesh_IncrementalMesh : public RefCounted {
    GDCLASS(ocgd_BRepMesh_IncrementalMesh, RefCounted);

protected:
    static void _bind_methods();

private:
    BRepMesh_IncrementalMesh* _mesh;
    bool _owns_mesh;
    bool _compute_normals;
    Ref<ocgd_TopoDS_Shape> _shape; // Store shape reference for normal computation

public:
    //! Default constructor
    ocgd_BRepMesh_IncrementalMesh();

    //! Destructor
    virtual ~ocgd_BRepMesh_IncrementalMesh();

    //! Constructor.
    //! Automatically calls method Perform.
    //! @param theShape shape to be meshed.
    //! @param theLinDeflection linear deflection.
    //! @param isRelative if TRUE deflection used for discretization of
    //! each edge will be <theLinDeflection> * <size of edge>. Deflection
    //! used for the faces will be the maximum deflection of their edges.
    //! @param theAngDeflection angular deflection.
    //! @param isInParallel if TRUE shape will be meshed in parallel.
    void init_with_shape(const Ref<ocgd_TopoDS_Shape>& shape,
                        double linear_deflection,
                        bool is_relative = false,
                        double angular_deflection = 0.5,
                        bool is_in_parallel = false,
                        bool compute_normals = true);

    //! Performs meshing of the shape.
    void perform();

    //! Compute normals for the meshed shape using enhanced algorithms
    void compute_normals();

    //! Returns whether normals should be computed automatically
    bool get_compute_normals() const;

    //! Set whether to compute normals automatically
    void set_compute_normals(bool compute_normals);

    //! Returns meshing parameters - linear deflection
    double get_linear_deflection() const;

    //! Set linear deflection
    void set_linear_deflection(double deflection);

    //! Returns meshing parameters - angular deflection
    double get_angular_deflection() const;

    //! Set angular deflection
    void set_angular_deflection(double deflection);

    //! Returns whether relative deflection is used
    bool get_relative_deflection() const;

    //! Set whether to use relative deflection
    void set_relative_deflection(bool is_relative);

    //! Returns whether parallel processing is enabled
    bool get_parallel_processing() const;

    //! Set whether to use parallel processing
    void set_parallel_processing(bool is_parallel);

    //! Returns modified flag.
    bool is_modified() const;

    //! Returns accumulated status flags faced during meshing.
    int get_status_flags() const;

    //! Returns minimum size parameter
    double get_min_size() const;

    //! Set minimum size parameter
    void set_min_size(double min_size);

    //! Returns interior linear deflection
    double get_interior_deflection() const;

    //! Set interior linear deflection
    void set_interior_deflection(double deflection);

    //! Returns interior angular deflection
    double get_interior_angular_deflection() const;

    //! Set interior angular deflection
    void set_interior_angular_deflection(double deflection);

    // Note: Static methods SetParallelDefault and IsParallelDefault from OpenCASCADE
    // are not exposed as they cannot be bound through Godot's ClassDB system

    // Internal access to the wrapped OpenCASCADE object
    BRepMesh_IncrementalMesh* get_occt_mesh() const { return _mesh; }
};

#endif // _ocgd_BRepMesh_IncrementalMesh_HeaderFile