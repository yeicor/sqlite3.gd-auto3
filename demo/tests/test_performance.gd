extends RefCounted

func run_test(db: SQLite3Database, log_func: Callable):
	log_func.call("Starting Performance Tests", "TEST_START")

	# Test bulk insert performance
	test_bulk_insert(db, log_func)

	# Test query performance
	test_query_performance(db, log_func)

	# Test prepared statement reuse
	test_prepared_reuse(db, log_func)

	# Test transaction performance
	test_transaction_performance(db, log_func)

	# Test index performance
	test_index_performance(db, log_func)

	log_func.call("Performance Tests completed", "TEST_END")

func test_bulk_insert(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing bulk insert performance", "SUBTEST")

	# Create a simple table for testing
	db.exec("CREATE TABLE perf_test (id INTEGER PRIMARY KEY, data TEXT)")

	var start_time = Time.get_ticks_usec()
	var count = 10000

	for i in range(count):
		var stmt = db.prepare("INSERT INTO perf_test (data) VALUES (?)")
		stmt.bind_text(1, "Data " + str(i))
		stmt.step()
		stmt.finalize()

	var duration = float(Time.get_ticks_usec() - start_time) / 1000000.0
	var rate = count / duration
	log_func.call("Inserted %d rows in %.3f seconds (%.1f inserts/sec)" % [count, duration, rate], "PERF")

func test_query_performance(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing query performance", "SUBTEST")

	var start_time = Time.get_ticks_usec()
	var count = 0

	var result_set = db.query("SELECT * FROM perf_test")
	while result_set.next():
		count += 1
	result_set.close()

	var duration = float(Time.get_ticks_usec() - start_time) / 1000000.0
	var rate = count / duration
	log_func.call("Queried %d rows in %.3f seconds (%.1f rows/sec)" % [count, duration, rate], "PERF")

func test_prepared_reuse(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing prepared statement reuse", "SUBTEST")

	var start_time = Time.get_ticks_usec()
	var count = 10000

	var stmt = db.prepare("INSERT INTO perf_test (data) VALUES (?)")
	for i in range(count):
		stmt.reset()
		stmt.clear_bindings()
		stmt.bind_text(1, "Reuse " + str(i))
		stmt.step()

	stmt.finalize()

	var duration = float(Time.get_ticks_usec() - start_time) / 1000000.0
	var rate = count / duration
	log_func.call("Inserted %d rows with reused statement in %.3f seconds (%.1f inserts/sec)" % [count, duration, rate], "PERF")

func test_transaction_performance(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing transaction performance", "SUBTEST")

	var start_time = Time.get_ticks_usec()
	var count = 1000

	db.exec("BEGIN")
	for i in range(count):
		db.exec("INSERT INTO perf_test (data) VALUES ('Txn " + str(i) + "')")
	db.exec("COMMIT")

	var duration = float(Time.get_ticks_usec() - start_time) / 1000000.0
	var rate = count / duration
	log_func.call("Inserted %d rows in transaction in %.3f seconds (%.1f inserts/sec)" % [count, duration, rate], "PERF")

func test_index_performance(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing index performance", "SUBTEST")

	# Create index
	db.exec("CREATE INDEX idx_perf_data ON perf_test(data)")

	var start_time = Time.get_ticks_usec()
	var count = 0

	var stmt = db.prepare("SELECT * FROM perf_test WHERE data LIKE ?")
	stmt.bind_text(1, "Data%")
	while stmt.step() == SQLite3Database.SQLITE_ROW:
		count += 1
	stmt.finalize()

	var duration = float(Time.get_ticks_usec() - start_time) / 1000000.0
	log_func.call("Indexed query found %d rows in %.3f seconds" % [count, duration], "PERF")

	# Clean up
	db.exec("DROP TABLE perf_test")
