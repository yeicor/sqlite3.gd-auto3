@tool
extends EditorPlugin

var importer_plugin

func _enter_tree():
	importer_plugin = preload("importer.gd").new()
	add_import_plugin(importer_plugin)


func _exit_tree():
	remove_import_plugin(importer_plugin)
	importer_plugin = null
