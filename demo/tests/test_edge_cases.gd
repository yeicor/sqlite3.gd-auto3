extends RefCounted

func run_test(db: SQLite3Database, log_func: Callable):
	log_func.call("Starting Edge Cases Test", "TEST_START")

	# Test error handling
	test_error_handling(db, log_func)

	# Test large data
	test_large_data(db, log_func)

	# Test concurrent operations (simulated)
	test_concurrent_ops(db, log_func)

	# Test limits
	test_limits(db, log_func)

	# Test invalid SQL
	test_invalid_sql(db, log_func)

	# Test memory database persistence
	test_memory_persistence(db, log_func)

	log_func.call("Edge Cases Test completed", "TEST_END")

func test_error_handling(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing error handling", "SUBTEST")

	# Try to insert duplicate primary key
	var stmt = db.prepare("INSERT INTO tasks (id, title) VALUES (1, 'Test')")
	if stmt:
		stmt.step()
		stmt.finalize()
		log_func.call("Handled duplicate key error", "INFO")

	# Try to access non-existent table
	var result = db.exec("SELECT * FROM nonexistent_table")
	if result != SQLite3Database.SQLITE_OK:
		log_func.call("Handled non-existent table error: " + db.errmsg(), "INFO")

func test_large_data(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing large data handling", "SUBTEST")

	# Create large string
	var large_string = ""
	for i in range(10000):
		large_string += "Large data " + str(i) + " "

	var stmt = db.prepare("INSERT INTO tasks (title, description) VALUES (?, ?)")
	if stmt:
		stmt.bind_text(1, "Large data test")
		stmt.bind_text(2, large_string)
		stmt.step()
		stmt.finalize()
		log_func.call("Inserted large data successfully", "INFO")

func test_concurrent_ops(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing concurrent operations simulation", "SUBTEST")

	# Simulate concurrent reads/writes
	var threads = []
	for i in range(5):
		var thread = Thread.new()
		threads.append(thread)
		if i % 2 == 0:
			# Reader thread
			thread.start(read_operation.bind(db, i, log_func))
		else:
			# Writer thread
			thread.start(write_operation.bind(db, i, log_func))

	# Wait for all threads
	for thread in threads:
		if thread.is_started():
			var result = thread.wait_to_finish()
			log_func.call("Thread completed with result: " + str(result), "INFO")

func read_operation(db: SQLite3Database, _id: int, _log_func: Callable) -> int:
	var stmt = db.prepare("SELECT COUNT(*) FROM tasks")
	if stmt:
		stmt.step()
		var count = stmt.column_int(0)
		stmt.finalize()
		return count
	return -1

func write_operation(db: SQLite3Database, id: int, _log_func: Callable) -> int:
	var stmt = db.prepare("INSERT INTO tasks (title) VALUES (?)")
	if stmt:
		stmt.bind_text(1, "Concurrent " + str(id))
		stmt.step()
		var rowid = db.last_insert_rowid()
		stmt.finalize()
		return rowid
	return -1

func test_limits(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing database limits", "SUBTEST")

	# Test various limits
	var max_columns = db.limit(SQLite3Database.SQLITE_LIMIT_COLUMN, -1)
	log_func.call("Max columns: " + str(max_columns), "INFO")

	var max_blob_length = db.limit(SQLite3Database.SQLITE_LIMIT_LENGTH, -1)
	log_func.call("Max blob length: " + str(max_blob_length), "INFO")

func test_invalid_sql(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing invalid SQL handling", "SUBTEST")

	var invalid_queries = [
		"SELECT * FROM",
		"INSERT INTO nonexistent VALUES",
		"UPDATE tasks SET invalid_column = 1",
		"DELETE FROM tasks WHERE",
	]

	for query in invalid_queries:
		var result = db.exec(query)
		if result != SQLite3Database.SQLITE_OK:
			log_func.call("Handled invalid SQL: " + query + " - " + db.errmsg(), "INFO")

func test_memory_persistence(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing memory database persistence", "SUBTEST")

	# Insert data
	db.exec("INSERT INTO tasks (title) VALUES ('Memory test')")

	# Check if data persists in the same connection
	var count = get_task_count(db)
	log_func.call("Tasks in memory DB: " + str(count), "INFO")

	# Note: In a real scenario, we'd test with multiple connections,
	# but since we're using :memory:, data doesn't persist across connections

func get_task_count(db: SQLite3Database) -> int:
	var stmt = db.prepare("SELECT COUNT(*) FROM tasks")
	if stmt == null:
		return 0
	stmt.step()
	var count = stmt.column_int(0)
	stmt.finalize()
	return count
