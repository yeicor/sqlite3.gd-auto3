/**
 * ocgd_TopoDS_Shape.cpp
 *
 * Godot GDExtension wrapper implementation for OpenCASCADE TopoDS_Shape class.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_TopoDS_Shape.hxx"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/TopAbs_ShapeEnum.hxx>
#include <opencascade/TopAbs_Orientation.hxx>
#include <opencascade/TopLoc_Location.hxx>
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/gp_Trsf.hxx>
#include <opencascade/Standard_Failure.hxx>

using namespace godot;

ocgd_TopoDS_Shape::ocgd_TopoDS_Shape() {
    try {
        _shape = TopoDS_Shape(); // Creates a NULL shape
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Failed to create shape - " + String(e.GetMessageString()));
        _shape = TopoDS_Shape(); // Ensure we have a null shape
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Failed to create shape - " + String(e.what()));
        _shape = TopoDS_Shape();
    }
}

ocgd_TopoDS_Shape::ocgd_TopoDS_Shape(const TopoDS_Shape& shape) {
    try {
        _shape = shape;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Failed to copy shape - " + String(e.GetMessageString()));
        _shape = TopoDS_Shape(); // Create null shape on error
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Failed to copy shape - " + String(e.what()));
        _shape = TopoDS_Shape();
    }
}

ocgd_TopoDS_Shape::~ocgd_TopoDS_Shape() {
    // TopoDS_Shape handles its own memory management
}

bool ocgd_TopoDS_Shape::is_null() const {
    try {
        return _shape.IsNull() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error checking if shape is null - " + String(e.GetMessageString()));
        return true; // Safe assumption if we can't check
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error checking if shape is null - " + String(e.what()));
        return true;
    }
}

void ocgd_TopoDS_Shape::nullify() {
    try {
        _shape.Nullify();
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error nullifying shape - " + String(e.GetMessageString()));
        // Try to create a new null shape as fallback
        try {
            _shape = TopoDS_Shape();
        } catch (...) {
            // If even this fails, there's not much we can do
        }
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error nullifying shape - " + String(e.what()));
        try {
            _shape = TopoDS_Shape();
        } catch (...) {
            // Fallback failed
        }
    }
}

int ocgd_TopoDS_Shape::shape_type() const {
    try {
        if (_shape.IsNull()) {
            return static_cast<int>(TopAbs_SHAPE);
        }
        return static_cast<int>(_shape.ShapeType());
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error getting shape type - " + String(e.GetMessageString()));
        return static_cast<int>(TopAbs_SHAPE); // Return generic shape type on error
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error getting shape type - " + String(e.what()));
        return static_cast<int>(TopAbs_SHAPE);
    }
}

int ocgd_TopoDS_Shape::orientation() const {
    try {
        if (_shape.IsNull()) {
            UtilityFunctions::printerr("TopoDS_Shape: Cannot get orientation of null shape");
            return static_cast<int>(TopAbs_FORWARD); // Default orientation
        }
        return static_cast<int>(_shape.Orientation());
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error getting orientation - " + String(e.GetMessageString()));
        return static_cast<int>(TopAbs_FORWARD);
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error getting orientation - " + String(e.what()));
        return static_cast<int>(TopAbs_FORWARD);
    }
}

void ocgd_TopoDS_Shape::set_orientation(int orient) {
    try {
        if (_shape.IsNull()) {
            UtilityFunctions::printerr("TopoDS_Shape: Cannot set orientation of null shape");
            return;
        }
        
        // Validate orientation value
        if (orient < TopAbs_FORWARD || orient > TopAbs_EXTERNAL) {
            UtilityFunctions::printerr("TopoDS_Shape: Invalid orientation value: " + String::num(orient));
            return;
        }
        
        _shape.Orientation(static_cast<TopAbs_Orientation>(orient));
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error setting orientation - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error setting orientation - " + String(e.what()));
    }
}

Vector3 ocgd_TopoDS_Shape::location() const {
    try {
        if (_shape.IsNull()) {
            UtilityFunctions::printerr("TopoDS_Shape: Cannot get location of null shape");
            return Vector3(0, 0, 0);
        }
        
        const TopLoc_Location& loc = _shape.Location();
        const gp_Trsf& trsf = loc.Transformation();
        const gp_XYZ& translation = trsf.TranslationPart();
        
        return Vector3(
            static_cast<float>(translation.X()),
            static_cast<float>(translation.Y()),
            static_cast<float>(translation.Z())
        );
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error getting location - " + String(e.GetMessageString()));
        return Vector3(0, 0, 0);
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error getting location - " + String(e.what()));
        return Vector3(0, 0, 0);
    }
}

bool ocgd_TopoDS_Shape::is_same(const Ref<ocgd_TopoDS_Shape>& other) const {
    try {
        if (other.is_null()) {
            UtilityFunctions::printerr("TopoDS_Shape: Cannot compare with null shape reference");
            return false;
        }
        
        if (_shape.IsNull() || other->get_occt_shape().IsNull()) {
            return false; // Two null shapes are not considered "same"
        }
        
        return _shape.IsSame(other->get_occt_shape()) == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error comparing shapes - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error comparing shapes - " + String(e.what()));
        return false;
    }
}

bool ocgd_TopoDS_Shape::is_equal(const Ref<ocgd_TopoDS_Shape>& other) const {
    try {
        if (other.is_null()) {
            UtilityFunctions::printerr("TopoDS_Shape: Cannot compare with null shape reference");
            return false;
        }
        
        if (_shape.IsNull() && other->get_occt_shape().IsNull()) {
            return true; // Two null shapes are equal
        }
        
        if (_shape.IsNull() || other->get_occt_shape().IsNull()) {
            return false; // One null, one non-null
        }
        
        return _shape.IsEqual(other->get_occt_shape()) == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error checking equality - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error checking equality - " + String(e.what()));
        return false;
    }
}

bool ocgd_TopoDS_Shape::is_valid() const {
    try {
        if (_shape.IsNull()) {
            return false;
        }
        
        // Basic validity check - shape should have a valid TShape
        return !_shape.TShape().IsNull();
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error checking validity - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error checking validity - " + String(e.what()));
        return false;
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_TopoDS_Shape::oriented(int orient) const {
    try {
        if (_shape.IsNull()) {
            UtilityFunctions::printerr("TopoDS_Shape: Cannot orient null shape");
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        // Validate orientation value
        if (orient < TopAbs_FORWARD || orient > TopAbs_EXTERNAL) {
            UtilityFunctions::printerr("TopoDS_Shape: Invalid orientation value: " + String::num(orient));
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
        TopoDS_Shape oriented_shape = _shape.Oriented(static_cast<TopAbs_Orientation>(orient));
        result->set_occt_shape(oriented_shape);
        return result;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error creating oriented shape - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error creating oriented shape - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_TopoDS_Shape::located(const Vector3& loc) const {
    try {
        if (_shape.IsNull()) {
            UtilityFunctions::printerr("TopoDS_Shape: Cannot locate null shape");
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        // Validate location vector
        if (!std::isfinite(loc.x) || !std::isfinite(loc.y) || !std::isfinite(loc.z)) {
            UtilityFunctions::printerr("TopoDS_Shape: Invalid location vector - contains non-finite values");
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        gp_Trsf trsf;
        gp_Vec vec(loc.x, loc.y, loc.z);
        trsf.SetTranslation(vec);
        TopLoc_Location location(trsf);
        
        Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
        TopoDS_Shape located_shape = _shape.Located(location);
        result->set_occt_shape(located_shape);
        return result;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error creating located shape - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error creating located shape - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_TopoDS_Shape::moved(const Vector3& loc) const {
    try {
        if (_shape.IsNull()) {
            UtilityFunctions::printerr("TopoDS_Shape: Cannot move null shape");
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        // Validate location vector
        if (!std::isfinite(loc.x) || !std::isfinite(loc.y) || !std::isfinite(loc.z)) {
            UtilityFunctions::printerr("TopoDS_Shape: Invalid move vector - contains non-finite values");
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        gp_Trsf trsf;
        gp_Vec vec(loc.x, loc.y, loc.z);
        trsf.SetTranslation(vec);
        TopLoc_Location location(trsf);
        
        Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
        TopoDS_Shape moved_shape = _shape.Moved(location);
        result->set_occt_shape(moved_shape);
        return result;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error moving shape - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error moving shape - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_TopoDS_Shape::reversed() const {
    try {
        if (_shape.IsNull()) {
            UtilityFunctions::printerr("TopoDS_Shape: Cannot reverse null shape");
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
        TopoDS_Shape reversed_shape = _shape.Reversed();
        result->set_occt_shape(reversed_shape);
        return result;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error reversing shape - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error reversing shape - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_TopoDS_Shape::complemented() const {
    try {
        if (_shape.IsNull()) {
            UtilityFunctions::printerr("TopoDS_Shape: Cannot complement null shape");
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
        TopoDS_Shape complemented_shape = _shape.Complemented();
        result->set_occt_shape(complemented_shape);
        return result;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error complementing shape - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error complementing shape - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_TopoDS_Shape::empty_copy() const {
    try {
        if (_shape.IsNull()) {
            UtilityFunctions::printerr("TopoDS_Shape: Cannot create empty copy of null shape");
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
        TopoDS_Shape shape_copy = _shape;
        shape_copy.EmptyCopy();
        result->set_occt_shape(shape_copy);
        return result;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error creating empty copy - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error creating empty copy - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

int ocgd_TopoDS_Shape::hash_code() const {
    try {
        // Use pointer address of TShape as a simple hash
        if (_shape.IsNull()) {
            return 0;
        }
        
        const Handle(TopoDS_TShape)& tshape = _shape.TShape();
        if (tshape.IsNull()) {
            return 0;
        }
        
        return static_cast<int>(reinterpret_cast<uintptr_t>(tshape.get()) % INT_MAX);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error computing hash code - " + String(e.GetMessageString()));
        return 0;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopoDS_Shape: Error computing hash code - " + String(e.what()));
        return 0;
    }
}

void ocgd_TopoDS_Shape::_bind_methods() {
    // Basic shape methods
    ClassDB::bind_method(D_METHOD("is_null"), &ocgd_TopoDS_Shape::is_null);
    ClassDB::bind_method(D_METHOD("nullify"), &ocgd_TopoDS_Shape::nullify);
    ClassDB::bind_method(D_METHOD("shape_type"), &ocgd_TopoDS_Shape::shape_type);
    ClassDB::bind_method(D_METHOD("orientation"), &ocgd_TopoDS_Shape::orientation);
    ClassDB::bind_method(D_METHOD("set_orientation", "orient"), &ocgd_TopoDS_Shape::set_orientation);
    ClassDB::bind_method(D_METHOD("location"), &ocgd_TopoDS_Shape::location);
    
    // Comparison methods
    ClassDB::bind_method(D_METHOD("is_same", "other"), &ocgd_TopoDS_Shape::is_same);
    ClassDB::bind_method(D_METHOD("is_equal", "other"), &ocgd_TopoDS_Shape::is_equal);
    ClassDB::bind_method(D_METHOD("is_valid"), &ocgd_TopoDS_Shape::is_valid);
    
    // Transformation methods
    ClassDB::bind_method(D_METHOD("oriented", "orient"), &ocgd_TopoDS_Shape::oriented);
    ClassDB::bind_method(D_METHOD("located", "loc"), &ocgd_TopoDS_Shape::located);
    ClassDB::bind_method(D_METHOD("moved", "loc"), &ocgd_TopoDS_Shape::moved);
    ClassDB::bind_method(D_METHOD("reversed"), &ocgd_TopoDS_Shape::reversed);
    ClassDB::bind_method(D_METHOD("complemented"), &ocgd_TopoDS_Shape::complemented);
    ClassDB::bind_method(D_METHOD("empty_copy"), &ocgd_TopoDS_Shape::empty_copy);
    ClassDB::bind_method(D_METHOD("hash_code"), &ocgd_TopoDS_Shape::hash_code);
    
    // Bind shape type enum constants
    BIND_ENUM_CONSTANT(COMPOUND);
    BIND_ENUM_CONSTANT(COMPSOLID);
    BIND_ENUM_CONSTANT(SOLID);
    BIND_ENUM_CONSTANT(SHELL);
    BIND_ENUM_CONSTANT(FACE);
    BIND_ENUM_CONSTANT(WIRE);
    BIND_ENUM_CONSTANT(EDGE);
    BIND_ENUM_CONSTANT(VERTEX);
    BIND_ENUM_CONSTANT(SHAPE);
    
    // Bind orientation enum constants
    BIND_ENUM_CONSTANT(FORWARD);
    BIND_ENUM_CONSTANT(REVERSED);
    BIND_ENUM_CONSTANT(INTERNAL);
    BIND_ENUM_CONSTANT(EXTERNAL);
}