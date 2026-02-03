#pragma once

#include <godot_cpp/classes/editor_import_plugin.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

class StepIgesBRepImporter final : public RefCounted
{
    GDCLASS(StepIgesBRepImporter, RefCounted)

protected:
    static void _bind_methods();

public:
    StepIgesBRepImporter();

    ~StepIgesBRepImporter() override;

    Error import(const String& p_source_file) const;
};
