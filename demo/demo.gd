extends Node

@onready var log_label: Label = $ScrollContainer/LogLabel

var test_scripts = [
	"res://tests/test_basic.gd",
	"res://tests/test_advanced.gd",
	"res://tests/test_performance.gd",
	"res://tests/test_edge_cases.gd"
]

func _log(msg: String, level: String = "INFO"):
	var timestamp = Time.get_datetime_string_from_system()
	var formatted_msg = "[%s] [%s] %s" % [timestamp, level, msg]
	print(formatted_msg)
	if log_label:
		log_label.text += formatted_msg + "\n"

func _ready():
	var start_time = Time.get_ticks_usec()
	_log("Starting Comprehensive SQLite3 Demo: Modular Test Suite", "START")

	# Open database
	var db = SQLite3Database.open(":memory:")
	if db == null:
		_log("Failed to open database", "ERROR")
		return
	_log("Database opened successfully", "SUCCESS")

	# Run all tests sequentially
	for script_path in test_scripts:
		run_test_script(script_path, db)

	# Close database
	db.close()
	_log("Database closed", "END")
	var duration = float(Time.get_ticks_usec() - start_time) / 1000000.0
	_log("Demo finished successfully in %.3f seconds!" % duration, "END")

func run_test_script(script_path: String, db: SQLite3Database):
	_log("Loading test script: " + script_path, "LOAD")
	var script = load(script_path)
	if script == null:
		_log("Failed to load script: " + script_path, "ERROR")
		return

	var instance = script.new()
	if not instance.has_method("run_test"):
		_log("Script does not have run_test method: " + script_path, "ERROR")
		return

	_log("Running test: " + script_path.get_basename(), "RUN")
	var test_start = Time.get_ticks_usec()
	instance.run_test(db, Callable(self, "_log"))
	var test_duration = float(Time.get_ticks_usec() - test_start) / 1000000.0
	_log("Test completed in %.3f seconds: " % test_duration + script_path.get_basename(), "SUCCESS")
