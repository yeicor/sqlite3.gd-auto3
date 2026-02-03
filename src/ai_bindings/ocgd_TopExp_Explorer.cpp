/**
 * ocgd_TopExp_Explorer.cpp
 *
 * Godot GDExtension wrapper implementation for OpenCASCADE TopExp_Explorer class.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_TopExp_Explorer.hxx"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/TopAbs_ShapeEnum.hxx>
#include <opencascade/Standard_Failure.hxx>

using namespace godot;

ocgd_TopExp_Explorer::ocgd_TopExp_Explorer() {
    try {
        _explorer = TopExp_Explorer();
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Failed to create explorer - " + String(e.GetMessageString()));
        // _explorer should be in a valid state even if default construction fails
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Failed to create explorer - " + String(e.what()));
    }
}

ocgd_TopExp_Explorer::~ocgd_TopExp_Explorer() {
    // TopExp_Explorer handles its own memory management
}

void ocgd_TopExp_Explorer::init(const Ref<ocgd_TopoDS_Shape>& shape, int to_find, int to_avoid) {
    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("TopExp_Explorer: Cannot initialize with null shape reference");
            return;
        }
        
        if (shape->is_null()) {
            UtilityFunctions::printerr("TopExp_Explorer: Cannot initialize with null shape");
            return;
        }
        
        // Validate shape types
        if (to_find < TopAbs_COMPOUND || to_find > TopAbs_SHAPE) {
            UtilityFunctions::printerr("TopExp_Explorer: Invalid to_find shape type: " + String::num(to_find));
            return;
        }
        
        if (to_avoid < TopAbs_COMPOUND || to_avoid > TopAbs_SHAPE) {
            UtilityFunctions::printerr("TopExp_Explorer: Invalid to_avoid shape type: " + String::num(to_avoid));
            return;
        }
        
        // Check for logical consistency
        if (to_find == TopAbs_SHAPE) {
            UtilityFunctions::printerr("TopExp_Explorer: Cannot search for TopAbs_SHAPE type");
            return;
        }
        
        const TopoDS_Shape& occt_shape = shape->get_occt_shape();
        
        _explorer.Init(
            occt_shape,
            static_cast<TopAbs_ShapeEnum>(to_find),
            static_cast<TopAbs_ShapeEnum>(to_avoid)
        );
        
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Failed to initialize - " + String(e.GetMessageString()));
        // Clear the explorer to a safe state
        try {
            _explorer.Clear();
        } catch (...) {
            // If clear fails, create a new explorer
            _explorer = TopExp_Explorer();
        }
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Failed to initialize - " + String(e.what()));
        try {
            _explorer.Clear();
        } catch (...) {
            _explorer = TopExp_Explorer();
        }
    }
}

bool ocgd_TopExp_Explorer::more() const {
    try {
        return _explorer.More() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error checking if more shapes available - " + String(e.GetMessageString()));
        return false; // Safe assumption
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error checking if more shapes available - " + String(e.what()));
        return false;
    }
}

void ocgd_TopExp_Explorer::next() {
    try {
        if (!more()) {
            UtilityFunctions::printerr("TopExp_Explorer: No more shapes to explore");
            return;
        }
        
        _explorer.Next();
        
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error advancing to next shape - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error advancing to next shape - " + String(e.what()));
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_TopExp_Explorer::current() const {
    try {
        if (!more()) {
            UtilityFunctions::printerr("TopExp_Explorer: No current shape available");
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        TopoDS_Shape current_shape = _explorer.Current();
        
        if (current_shape.IsNull()) {
            UtilityFunctions::printerr("TopExp_Explorer: Current shape is null");
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
        result->set_occt_shape(current_shape);
        return result;
        
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error getting current shape - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error getting current shape - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_TopExp_Explorer::value() const {
    // value() is an alias for current()
    return current();
}

void ocgd_TopExp_Explorer::re_init() {
    try {
        _explorer.ReInit();
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error reinitializing explorer - " + String(e.GetMessageString()));
        // Try to clear and create new explorer as fallback
        try {
            _explorer.Clear();
            _explorer = TopExp_Explorer();
        } catch (...) {
            UtilityFunctions::printerr("TopExp_Explorer: Failed to recover from reinit error");
        }
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error reinitializing explorer - " + String(e.what()));
        try {
            _explorer.Clear();
            _explorer = TopExp_Explorer();
        } catch (...) {
            UtilityFunctions::printerr("TopExp_Explorer: Failed to recover from reinit error");
        }
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_TopExp_Explorer::explored_shape() const {
    try {
        TopoDS_Shape explored = _explorer.ExploredShape();
        
        if (explored.IsNull()) {
            UtilityFunctions::printerr("TopExp_Explorer: No shape being explored");
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
        result->set_occt_shape(explored);
        return result;
        
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error getting explored shape - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error getting explored shape - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

int ocgd_TopExp_Explorer::depth() const {
    try {
        return _explorer.Depth();
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error getting exploration depth - " + String(e.GetMessageString()));
        return 0; // Return 0 as safe default
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error getting exploration depth - " + String(e.what()));
        return 0;
    }
}

void ocgd_TopExp_Explorer::clear() {
    try {
        _explorer.Clear();
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error clearing explorer - " + String(e.GetMessageString()));
        // Try to create a new explorer as fallback
        try {
            _explorer = TopExp_Explorer();
        } catch (...) {
            UtilityFunctions::printerr("TopExp_Explorer: Failed to recover from clear error");
        }
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopExp_Explorer: Error clearing explorer - " + String(e.what()));
        try {
            _explorer = TopExp_Explorer();
        } catch (...) {
            UtilityFunctions::printerr("TopExp_Explorer: Failed to recover from clear error");
        }
    }
}

void ocgd_TopExp_Explorer::_bind_methods() {
    // Initialization and control methods
    ClassDB::bind_method(D_METHOD("init", "shape", "to_find", "to_avoid"), &ocgd_TopExp_Explorer::init, DEFVAL(TopAbs_SHAPE));
    ClassDB::bind_method(D_METHOD("more"), &ocgd_TopExp_Explorer::more);
    ClassDB::bind_method(D_METHOD("next"), &ocgd_TopExp_Explorer::next);
    ClassDB::bind_method(D_METHOD("current"), &ocgd_TopExp_Explorer::current);
    ClassDB::bind_method(D_METHOD("value"), &ocgd_TopExp_Explorer::value);
    ClassDB::bind_method(D_METHOD("re_init"), &ocgd_TopExp_Explorer::re_init);
    ClassDB::bind_method(D_METHOD("explored_shape"), &ocgd_TopExp_Explorer::explored_shape);
    ClassDB::bind_method(D_METHOD("depth"), &ocgd_TopExp_Explorer::depth);
    ClassDB::bind_method(D_METHOD("clear"), &ocgd_TopExp_Explorer::clear);
    
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
}