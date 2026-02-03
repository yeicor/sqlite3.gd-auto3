# Custom resource to hold CAD data
class_name CADResource
extends Resource

@export var source_file: String
@export var import_options: Dictionary
var shapes
@export var meshes: Array[ArrayMesh]
@export var mesh_materials: Array[Material]
@export var metadata: Dictionary
@export var colors: Dictionary
@export var materials: Dictionary
@export var assembly_structure: Dictionary
@export var analysis_results: Dictionary

func get_shape_count() -> int:
	return shapes.size()

func get_shape(index: int) -> ocgd_TopoDS_Shape:
	if index >= 0 and index < shapes.size():
		return shapes[index]
	return null

func get_mesh(index: int) -> ArrayMesh:
	if index >= 0 and index < meshes.size():
		return meshes[index]
	return null

func get_material(index: int) -> Material:
	if index >= 0 and index < mesh_materials.size():
		return mesh_materials[index]
	return null
