extends RefCounted

func run_test(db: SQLite3Database, log_func: Callable):
	log_func.call("Starting Advanced Database Features Test", "TEST_START")

	# Create view
	var view_sql = """
	CREATE VIEW pending_tasks AS
	SELECT id, title, description, priority
	FROM tasks
	WHERE done = 0
	ORDER BY priority DESC, created_at ASC
	"""
	var result = db.exec(view_sql)
	if result == SQLite3Database.SQLITE_OK:
		log_func.call("View 'pending_tasks' created", "SUCCESS")
	else:
		log_func.call("Failed to create view: " + db.errmsg(), "ERROR")

	# Create trigger
	var trigger_sql = """
	CREATE TRIGGER update_task_timestamp
	AFTER UPDATE ON tasks
	BEGIN
		UPDATE tasks SET updated_at = CURRENT_TIMESTAMP WHERE id = NEW.id;
	END
	"""
	result = db.exec(trigger_sql)
	if result == SQLite3Database.SQLITE_OK:
		log_func.call("Trigger 'update_task_timestamp' created", "SUCCESS")
	else:
		log_func.call("Failed to create trigger: " + db.errmsg(), "ERROR")

	# Test transactions
	test_transactions(db, log_func)

	# Test blob operations
	test_blobs(db, log_func)

	# Test prepared statements with search
	test_search(db, log_func)

	# Test WAL mode
	test_wal(db, log_func)

	# Test database status
	test_status(db, log_func)

	log_func.call("Advanced Database Features Test completed", "TEST_END")

func test_transactions(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing transactions", "SUBTEST")

	# Start transaction
	db.exec("BEGIN")
	log_func.call("Transaction started", "INFO")

	# Insert temporary task
	var _temp_id = insert_temp_task(db, "Temp task", log_func)
	log_func.call("Inserted temp task in transaction", "INFO")

	# Rollback
	db.exec("ROLLBACK")
	log_func.call("Transaction rolled back", "INFO")

	# Check if temp task is gone
	var count = get_task_count(db)
	log_func.call("Task count after rollback: " + str(count), "INFO")

	# Commit a real task
	db.exec("BEGIN")
	var _real_id = insert_temp_task(db, "Real task", log_func)
	db.exec("COMMIT")
	log_func.call("Transaction committed", "INFO")

	count = get_task_count(db)
	log_func.call("Task count after commit: " + str(count), "INFO")

func insert_temp_task(db: SQLite3Database, title: String, log_func: Callable) -> int:
	var stmt = db.prepare("INSERT INTO tasks (title, priority) VALUES (?, 1)")
	if stmt == null:
		log_func.call("Failed to prepare temp insert", "ERROR")
		return -1
	stmt.bind_text(1, title)
	stmt.step()
	var id = db.last_insert_rowid()
	stmt.finalize()
	return id

func get_task_count(db: SQLite3Database) -> int:
	var stmt = db.prepare("SELECT COUNT(*) FROM tasks")
	if stmt == null:
		return 0
	stmt.step()
	var count = stmt.column_int(0)
	stmt.finalize()
	return count

func test_blobs(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing BLOB operations", "SUBTEST")

	var blob_data = PackedByteArray([72, 101, 108, 108, 111, 32, 87, 111, 114, 108, 100])  # "Hello World"
	var stmt = db.prepare("INSERT INTO tasks (title, description) VALUES (?, ?)")
	if stmt == null:
		log_func.call("Failed to prepare blob insert", "ERROR")
		return
	stmt.bind_text(1, "Blob test")
	stmt.bind_blob(2, blob_data)
	stmt.step()
	stmt.finalize()
	log_func.call("Inserted blob data", "INFO")

	var query_stmt = db.prepare("SELECT description FROM tasks WHERE title = ?")
	if query_stmt == null:
		log_func.call("Failed to prepare blob query", "ERROR")
		return
	query_stmt.bind_text(1, "Blob test")
	if query_stmt.step() == SQLite3Database.SQLITE_ROW:
		var retrieved = query_stmt.column_blob(0)
		log_func.call("Retrieved blob: " + retrieved.get_string_from_utf8(), "INFO")
	query_stmt.finalize()

func test_search(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing search functionality", "SUBTEST")

	var stmt = db.prepare("SELECT id, title FROM tasks WHERE title LIKE ?")
	if stmt == null:
		log_func.call("Failed to prepare search statement", "ERROR")
		return
	stmt.bind_text(1, "%project%")
	while stmt.step() == SQLite3Database.SQLITE_ROW:
		var title = stmt.column_text(1)
		log_func.call("Found: " + title, "INFO")
	stmt.finalize()

func test_wal(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing WAL mode", "SUBTEST")

	var result = db.exec("PRAGMA journal_mode = WAL")
	if result == SQLite3Database.SQLITE_OK:
		log_func.call("WAL mode enabled", "SUCCESS")
	else:
		log_func.call("Failed to enable WAL mode: " + db.errmsg(), "ERROR")

func test_status(db: SQLite3Database, log_func: Callable):
	log_func.call("Testing database status", "SUBTEST")

	var status = db.db_status(0)  # SQLITE_DBSTATUS_LOOKASIDE_USED
	log_func.call("Lookaside used: " + str(status[1]) + "/" + str(status[2]), "INFO")

	var cache_size = db.exec("PRAGMA cache_size")
	log_func.call("Cache size: " + str(cache_size), "INFO")
