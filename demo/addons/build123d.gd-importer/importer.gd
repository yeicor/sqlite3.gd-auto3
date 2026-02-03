@tool
extends EditorImportPlugin

# Import presets
enum Preset {
	DEFAULT,
	HIGH_QUALITY,
	PERFORMANCE,
	MANUFACTURING
}

# Supported CAD formats
const SUPPORTED_EXTENSIONS = ["step", "stp", "iges", "igs", "brep", "stl"]

func _get_importer_name() -> String:
	return "build123d.gd-importer"

func _get_visible_name() -> String:
	return "build123d CAD Model"

func _get_recognized_extensions() -> PackedStringArray:
	return PackedStringArray(SUPPORTED_EXTENSIONS)

func _get_save_extension() -> String:
	return "res"

func _get_resource_type() -> String:
	return "Resource"

func _get_preset_count() -> int:
	return Preset.size()

func _get_preset_name(preset_index: int) -> String:
	match preset_index:
		Preset.DEFAULT:
			return "Default"
		Preset.HIGH_QUALITY:
			return "High Quality"
		Preset.PERFORMANCE:
			return "Performance"
		Preset.MANUFACTURING:
			return "Manufacturing Analysis"
		_:
			return "Unknown"

func _get_priority() -> float:
	return 1.0

func _get_import_order() -> int:
	return 0

func _can_import_threaded() -> bool:
	return true

func _get_import_options(path: String, preset_index: int) -> Array[Dictionary]:
	var options: Array[Dictionary] = []

	# File import options
	options.append({
		"name": "import/format",
		"default_value": 0,
		"property_hint": PROPERTY_HINT_ENUM,
		"hint_string": "Auto-detect,STEP,IGES,BREP,STL"
	})

	options.append({
		"name": "import/mode",
		"default_value": 1,
		"property_hint": PROPERTY_HINT_ENUM,
		"hint_string": "Single Shape,Assembly,Separate Shapes"
	})

	options.append({
		"name": "import/units",
		"default_value": 5,
		"property_hint": PROPERTY_HINT_ENUM,
		"hint_string": "Millimeter,Centimeter,Meter,Inch,Foot,Auto"
	})

	options.append({
		"name": "import/scaling_factor",
		"default_value": 1.0,
		"property_hint": PROPERTY_HINT_RANGE,
		"hint_string": "0.001,1000.0,0.001"
	})

	# Metadata options
	options.append({
		"name": "metadata/read_colors",
		"default_value": true
	})

	options.append({
		"name": "metadata/read_materials",
		"default_value": true
	})

	options.append({
		"name": "metadata/read_layers",
		"default_value": true
	})

	options.append({
		"name": "metadata/read_assembly_structure",
		"default_value": true
	})

	options.append({
		"name": "metadata/read_custom_properties",
		"default_value": true
	})

	# Mesh generation options
	options.append({
		"name": "mesh/generate_mesh",
		"default_value": true
	})

	options.append({
		"name": "mesh/linear_deflection",
		"default_value": 0.1,
		"property_hint": PROPERTY_HINT_RANGE,
		"hint_string": "0.001,10.0,0.001"
	})

	options.append({
		"name": "mesh/angular_deflection",
		"default_value": 0.5,
		"property_hint": PROPERTY_HINT_RANGE,
		"hint_string": "0.01,3.14159,0.01"
	})

	options.append({
		"name": "mesh/relative_deflection",
		"default_value": false
	})

	options.append({
		"name": "mesh/parallel_processing",
		"default_value": true
	})

	options.append({
		"name": "mesh/include_normals",
		"default_value": true
	})

	options.append({
		"name": "mesh/include_uvs",
		"default_value": false
	})

	options.append({
		"name": "mesh/merge_vertices",
		"default_value": true
	})

	# Geometry options
	options.append({
		"name": "geometry/repair_level",
		"default_value": 1,
		"property_hint": PROPERTY_HINT_ENUM,
		"hint_string": "None,Basic,Advanced,Complete"
	})

	options.append({
		"name": "geometry/validate_geometry",
		"default_value": true
	})

	options.append({
		"name": "geometry/tolerance",
		"default_value": 1e-6,
		"property_hint": PROPERTY_HINT_RANGE,
		"hint_string": "1e-9,1e-3,1e-9"
	})

	# Analysis options
	options.append({
		"name": "analysis/compute_properties",
		"default_value": false
	})

	options.append({
		"name": "analysis/detect_features",
		"default_value": false
	})

	options.append({
		"name": "analysis/topology_analysis",
		"default_value": false
	})

	# Export options
	options.append({
		"name": "export/create_mesh_resources",
		"default_value": true
	})

	options.append({
		"name": "export/create_material_resources",
		"default_value": true
	})

	options.append({
		"name": "export/export_stl",
		"default_value": false
	})

	options.append({
		"name": "export/export_obj",
		"default_value": false
	})

	# Apply preset defaults
	_apply_preset_defaults(options, preset_index)

	return options

func _apply_preset_defaults(options: Array[Dictionary], preset_index: int) -> void:
	match preset_index:
		Preset.HIGH_QUALITY:
			_set_option_value(options, "mesh/linear_deflection", 0.01)
			_set_option_value(options, "mesh/angular_deflection", 0.1)
			_set_option_value(options, "mesh/include_normals", true)
			_set_option_value(options, "mesh/include_uvs", true)
			_set_option_value(options, "geometry/repair_level", 2)

		Preset.PERFORMANCE:
			_set_option_value(options, "mesh/linear_deflection", 0.5)
			_set_option_value(options, "mesh/angular_deflection", 1.0)
			_set_option_value(options, "mesh/parallel_processing", true)
			_set_option_value(options, "metadata/read_colors", false)
			_set_option_value(options, "metadata/read_materials", false)
			_set_option_value(options, "geometry/repair_level", 0)

		Preset.MANUFACTURING:
			_set_option_value(options, "analysis/compute_properties", true)
			_set_option_value(options, "analysis/detect_features", true)
			_set_option_value(options, "analysis/topology_analysis", true)
			_set_option_value(options, "geometry/repair_level", 3)
			_set_option_value(options, "geometry/validate_geometry", true)

func _set_option_value(options: Array[Dictionary], name: String, value: Variant) -> void:
	for option in options:
		if option["name"] == name:
			option["default_value"] = value
			break

func _get_option_visibility(path: String, option_name: StringName, options: Dictionary) -> bool:
	# Hide mesh options if mesh generation is disabled
	if not options.get("mesh/generate_mesh", true):
		if option_name.begins_with("mesh/") and option_name != "mesh/generate_mesh":
			return false

	# Hide UV options if normals are not included
	if option_name == "mesh/include_uvs":
		return options.get("mesh/include_normals", true)

	# Hide analysis options for STL files (limited metadata)
	var file_ext = path.get_extension().to_lower()
	if file_ext == "stl":
		if option_name.begins_with("metadata/") or option_name.begins_with("analysis/"):
			return false

	return true

func _import(source_file: String, save_path: String, options: Dictionary,
			platform_variants: Array[String], gen_files: Array[String]) -> Error:
	var source_path := ProjectSettings.globalize_path(source_file)
	print("Importing CAD file: ", source_path)

	# Create the main importer
	var importer = ocgd_CADFileImporter.new()

	# Configure importer based on options
	_configure_importer(importer, options)

	# Import the file
	var import_result = importer.import_file_with_metadata(source_path)
	if import_result.is_empty():
		push_error("Failed to import CAD file: " + importer.get_last_error())
		return ERR_FILE_CORRUPT

	# Create the main resource
	var cad_resource = CADResource.new()
	cad_resource.source_file = source_path
	cad_resource.import_options = options.duplicate()

	# Extract shapes
	var shapes = import_result.get("all_shapes", [])
	if shapes.is_empty():
		push_error("No shapes found in CAD file")
		return ERR_FILE_CORRUPT

	cad_resource.shapes = shapes

	# Extract metadata
	cad_resource.metadata = import_result.get("metadata", {})
	cad_resource.colors = import_result.get("colors", {})
	cad_resource.materials = import_result.get("materials", {})
	cad_resource.assembly_structure = import_result.get("assembly", {})

	# Generate meshes if requested
	if options.get("mesh/generate_mesh", true):
		_generate_meshes(cad_resource, options, gen_files, save_path)

	# Perform analysis if requested
	if options.get("analysis/compute_properties", false):
		_perform_analysis(cad_resource, options)

	# Export additional formats if requested
	_export_additional_formats(cad_resource, options, gen_files, save_path)

	# Save the main resource
	var save_result = ResourceSaver.save(cad_resource, save_path + ".res")
	if save_result != OK:
		push_error("Failed to save CAD resource")
		return save_result

	print("Successfully imported CAD file with ", shapes.size(), " shapes")
	return OK

func _configure_importer(importer: ocgd_CADFileImporter, options: Dictionary) -> void:
	# Set format
	var format = options.get("import/format", 0)
	importer.set_import_format(format)

	# Set import mode
	var mode = options.get("import/mode", 1)
	importer.set_import_mode(mode)

	# Set units
	var units = options.get("import/units", 5)
	importer.set_target_units(units)

	# Set scaling
	var scaling = options.get("import/scaling_factor", 1.0)
	importer.set_scaling_factor(scaling)

	# Set repair level
	var repair_level = options.get("geometry/repair_level", 1)
	importer.set_repair_level(repair_level)

	# Set tolerance
	var tolerance = options.get("geometry/tolerance", 1e-6)
	importer.set_tolerance(tolerance)

	# Set metadata options
	importer.set_read_colors(options.get("metadata/read_colors", true))
	importer.set_read_materials(options.get("metadata/read_materials", true))
	importer.set_read_layers(options.get("metadata/read_layers", true))
	importer.set_read_assembly_structure(options.get("metadata/read_assembly_structure", true))
	importer.set_read_metadata(options.get("metadata/read_custom_properties", true))
	importer.set_validate_geometry(options.get("geometry/validate_geometry", true))
	importer.set_fix_geometry(options.get("geometry/repair_level", 1) > 0)

func _generate_meshes(cad_resource: CADResource, options: Dictionary,
					 gen_files: Array[String], save_path: String) -> void:
	var mesh_generator = ocgd_BRepMesh_IncrementalMesh.new()
	var mesh_extractor = ocgd_MeshDataExtractor.new()

	# Configure mesh extractor
	mesh_extractor.set_include_normals(options.get("mesh/include_normals", true))
	mesh_extractor.set_include_uvs(options.get("mesh/include_uvs", false))
	mesh_extractor.set_merge_vertices(options.get("mesh/merge_vertices", true))

	var meshes: Array[ArrayMesh] = []
	var materials: Array[Material] = []

	for i in range(cad_resource.shapes.size()):
		var shape = cad_resource.shapes[i]

		# Generate triangulation
		mesh_generator.init_with_shape(
			shape,
			options.get("mesh/linear_deflection", 0.1),
			options.get("mesh/relative_deflection", false),
			options.get("mesh/angular_deflection", 0.5),
			options.get("mesh/parallel_processing", true)
		)
		mesh_generator.perform()
		
		# TODO: Make this debugging step optional...
		var debug_info = mesh_extractor.get_detailed_triangulation_info(shape)
		for face_info in debug_info["faces"]:
			if not face_info["is_valid"]:
				print("Face ", face_info["face_index"], " has issues: ", face_info["error"])
				print("Nodes: ", face_info["node_count"], " Triangles: ", face_info["triangle_count"])
				print("Array bounds - Nodes: [", face_info["nodes_lower"], ",", face_info["nodes_upper"], "]")

		# Extract mesh data
		var mesh_data = mesh_extractor.extract_mesh_data(shape)
		if mesh_data.is_empty():
			continue

		# Create Godot mesh
		var array_mesh = ArrayMesh.new()
		var arrays = []
		arrays.resize(Mesh.ARRAY_MAX)

		arrays[Mesh.ARRAY_VERTEX] = mesh_data.get("vertices", PackedVector3Array())
		arrays[Mesh.ARRAY_INDEX] = mesh_data.get("triangles", PackedInt32Array())

		if mesh_data.has("normals"):
			arrays[Mesh.ARRAY_NORMAL] = mesh_data["normals"]

		if mesh_data.has("uvs"):
			arrays[Mesh.ARRAY_TEX_UV] = mesh_data["uvs"]

		array_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
		meshes.append(array_mesh)

		# Create material if available
		var material = _create_material_from_cad_data(cad_resource, i)
		materials.append(material)

		# Save mesh as separate resource if requested
		if options.get("export/create_mesh_resources", true):
			var mesh_path = save_path + "_mesh_" + str(i) + ".res"
			ResourceSaver.save(array_mesh, mesh_path)
			gen_files.append(mesh_path)

	cad_resource.meshes = meshes
	cad_resource.mesh_materials = materials

func _create_material_from_cad_data(cad_resource: CADResource, shape_index: int) -> Material:
	var material = StandardMaterial3D.new()

	# Try to get color information
	var colors = cad_resource.colors
	if colors.has(str(shape_index)):
		var color_info = colors[str(shape_index)]
		if color_info is Color:
			material.albedo_color = color_info
		elif color_info is Dictionary and color_info.has("color"):
			material.albedo_color = color_info["color"]

	# Try to get material information
	var materials = cad_resource.materials
	if materials.has(str(shape_index)):
		var material_info = materials[str(shape_index)]
		if material_info is Dictionary:
			if material_info.has("roughness"):
				material.roughness = material_info["roughness"]
			if material_info.has("metallic"):
				material.metallic = material_info["metallic"]
			if material_info.has("emission"):
				material.emission = material_info["emission"]

	return material

func _perform_analysis(cad_resource: CADResource, options: Dictionary) -> void:
	var analyzer = ocgd_ShapeAnalyzer.new()
	var topology_analyzer = ocgd_TopologyAnalyzer.new()

	var analysis_results = {}

	for i in range(cad_resource.shapes.size()):
		var shape = cad_resource.shapes[i]
		var shape_analysis = {}

		if options.get("analysis/compute_properties", false):
			# Compute geometric properties
			shape_analysis["volume_properties"] = analyzer.compute_volume_properties(shape)
			shape_analysis["surface_properties"] = analyzer.compute_surface_properties(shape)
			shape_analysis["bounding_box"] = analyzer.get_bounding_box(shape)
			shape_analysis["center_of_mass"] = analyzer.get_center_of_mass(shape)

		if options.get("analysis/detect_features", false):
			# Detect geometric features
			shape_analysis["holes"] = topology_analyzer.detect_holes(shape)
			shape_analysis["fillets"] = topology_analyzer.detect_fillets(shape)
			shape_analysis["chamfers"] = topology_analyzer.detect_chamfers(shape)

		if options.get("analysis/topology_analysis", false):
			# Analyze topology
			shape_analysis["hierarchy"] = topology_analyzer.analyze_shape_hierarchy(shape)
			shape_analysis["connectivity"] = topology_analyzer.analyze_connectivity(shape)
			shape_analysis["validation"] = topology_analyzer.validate_topology(shape)

		analysis_results[str(i)] = shape_analysis

	cad_resource.analysis_results = analysis_results

func _export_additional_formats(cad_resource: CADResource, options: Dictionary,
								gen_files: Array[String], save_path: String) -> void:

	if options.get("export/export_stl", false):
		var stl_exporter = ocgd_STLExporter.new()
		for i in range(cad_resource.shapes.size()):
			var stl_path = save_path + "_shape_" + str(i) + ".stl"
			if stl_exporter.write_file(cad_resource.shapes[i], stl_path):
				gen_files.append(stl_path)

	if options.get("export/export_obj", false):
		var advanced_exporter = ocgd_AdvancedMeshExporter.new()
		for i in range(cad_resource.shapes.size()):
			var obj_path = save_path + "_shape_" + str(i) + ".obj"
			if advanced_exporter.export_to_obj(cad_resource.shapes[i], obj_path, true):
				gen_files.append(obj_path)
