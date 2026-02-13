extends Node

@onready var log_label: Label = $ScrollContainer/LogLabel

func _log(msg: String):
	print(msg)
	log_label.text += msg + "\n"

func _ready():
	var start_time = Time.get_ticks_usec()
	_log("Starting Comprehensive SQLite3 Demo: Advanced Database Operations")

	# Open database
	var db = SQLite3Database.open(":memory:")
	if db == null:
		_log("Failed to open database")
		return
	_log("Database opened successfully")

	# Enable foreign keys
	db.exec("PRAGMA foreign_keys = ON")
	_log("Foreign keys enabled")

	# Create main table
	var create_sql = """
	CREATE TABLE tasks (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		title TEXT NOT NULL,
		description TEXT,
		priority INTEGER DEFAULT 1,
		done INTEGER DEFAULT 0,
		created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
		updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
	)
	"""
	var result = db.exec(create_sql)
	if result != 0:
		_log("Failed to create table: " + db.errmsg())
		return
	_log("Table 'tasks' created")

	# Create categories table
	var create_cat_sql = """
	CREATE TABLE categories (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		name TEXT UNIQUE NOT NULL
	)
	"""
	db.exec(create_cat_sql)
	_log("Table 'categories' created")

	# Create task_categories junction table
	var create_junc_sql = """
	CREATE TABLE task_categories (
		task_id INTEGER,
		category_id INTEGER,
		FOREIGN KEY (task_id) REFERENCES tasks(id) ON DELETE CASCADE,
		FOREIGN KEY (category_id) REFERENCES categories(id) ON DELETE CASCADE,
		PRIMARY KEY (task_id, category_id)
	)
	"""
	db.exec(create_junc_sql)
	_log("Junction table 'task_categories' created")

	# Create index
	db.exec("CREATE INDEX idx_tasks_done ON tasks(done)")
	db.exec("CREATE INDEX idx_tasks_title ON tasks(title)")
	_log("Indexes created")

	# Create view
	var view_sql = """
	CREATE VIEW pending_tasks AS
	SELECT id, title, description, priority
	FROM tasks
	WHERE done = 0
	ORDER BY priority DESC, created_at ASC
	"""
	db.exec(view_sql)
	_log("View 'pending_tasks' created")

	# Create trigger
	var trigger_sql = """
	CREATE TRIGGER update_task_timestamp
	AFTER UPDATE ON tasks
	BEGIN
		UPDATE tasks SET updated_at = CURRENT_TIMESTAMP WHERE id = NEW.id;
	END
	"""
	db.exec(trigger_sql)
	_log("Trigger 'update_task_timestamp' created")

	# Insert categories
	insert_category(db, "Work")
	insert_category(db, "Personal")
	insert_category(db, "Shopping")

	# Insert tasks
	var task1_id = insert_task(db, "Buy groceries", "Milk, bread, eggs", 2)
	var task2_id = insert_task(db, "Finish project", "Complete the SQLite demo", 5)
	var task3_id = insert_task(db, "Exercise", "Go for a run", 3)

	# Assign categories
	assign_category(db, task1_id, 3)  # Shopping
	assign_category(db, task2_id, 1)  # Work
	assign_category(db, task3_id, 2)  # Personal

	# List all tasks
	list_tasks(db)

	# List pending tasks using view
	list_pending_tasks(db)

	# Mark first task as done
	mark_done(db, task1_id)

	# List again
	list_tasks(db)

	# Use prepared statement to search
	search_tasks(db, "project")

	# Update task priority
	update_priority(db, task3_id, 4)

	# List final
	list_tasks(db)

	# Test transactions
	test_transaction(db)

	# Test blob operations
	test_blob(db)

	# Test backup
	test_backup(db)

	# Test WAL mode
	test_wal(db)

	# Test database status
	log_database_status(db)

	# Close database
	db.close()
	_log("Database closed")
	_log("Demo finished successfully in "+str(float(Time.get_ticks_usec() - start_time) / 1000.0)+"ms!")
	if OS.get_environment("AUTO_CLOSE") != "":
		get_tree().quit()

func insert_category(db: SQLite3Database, cat_name: String) -> int:
	var stmt = db.prepare("INSERT INTO categories (name) VALUES (?)")
	if stmt == null:
		_log("Failed to prepare insert category statement")
		return -1
	stmt.bind_text(1, cat_name)
	var step_result = stmt.step()
	if step_result != 101:  # SQLITE_DONE
		_log("Failed to insert category: " + db.errmsg())
		stmt.finalize()
		return -1
	var id = db.last_insert_rowid()
	_log("Inserted category: " + cat_name + " (ID: " + str(id) + ")")
	stmt.finalize()
	return id

func assign_category(db: SQLite3Database, task_id: int, cat_id: int):
	var stmt = db.prepare("INSERT INTO task_categories (task_id, category_id) VALUES (?, ?)")
	if stmt == null:
		_log("Failed to prepare assign category statement")
		return
	stmt.bind_int(1, task_id)
	stmt.bind_int(2, cat_id)
	var step_result = stmt.step()
	if step_result != 101:
		_log("Failed to assign category: " + db.errmsg())
	else:
		_log("Assigned category " + str(cat_id) + " to task " + str(task_id))
	stmt.finalize()

func insert_task(db: SQLite3Database, title: String, description: String, priority: int) -> int:
	var stmt = db.prepare("INSERT INTO tasks (title, description, priority) VALUES (?, ?, ?)")
	if stmt == null:
		_log("Failed to prepare insert statement")
		return -1
	stmt.bind_text(1, title)
	stmt.bind_text(2, description)
	stmt.bind_int(3, priority)
	var step_result = stmt.step()
	if step_result != 101:  # SQLITE_DONE
		_log("Failed to insert task: " + db.errmsg())
		stmt.finalize()
		return -1
	var id = db.last_insert_rowid()
	_log("Inserted task: " + title + " (ID: " + str(id) + ")")
	stmt.finalize()
	return id

func list_tasks(db: SQLite3Database):
	_log("Listing all tasks:")
	var result_set = db.query("SELECT t.id, t.title, t.description, t.priority, t.done, t.created_at, t.updated_at, GROUP_CONCAT(c.name, ', ') as categories FROM tasks t LEFT JOIN task_categories tc ON t.id = tc.task_id LEFT JOIN categories c ON tc.category_id = c.id GROUP BY t.id")
	while result_set.next():
		var row = result_set.current_row()
		var status = "Done" if row["done"] == 1 else "Pending"
		var cats = row["categories"] if row["categories"] != null else "None"
		_log("ID: " + str(row["id"]) + ", Title: " + row["title"] + ", Priority: " + str(row["priority"]) + ", Status: " + status + ", Categories: " + cats)
	result_set.close()

func list_pending_tasks(db: SQLite3Database):
	_log("Listing pending tasks via view:")
	var result_set = db.query("SELECT * FROM pending_tasks")
	while result_set.next():
		var row = result_set.current_row()
		_log("ID: " + str(row["id"]) + ", Title: " + row["title"] + ", Priority: " + str(row["priority"]))
	result_set.close()

func mark_done(db: SQLite3Database, id: int):
	var stmt = db.prepare("UPDATE tasks SET done = 1 WHERE id = ?")
	if stmt == null:
		_log("Failed to prepare update statement")
		return
	stmt.bind_int(1, id)
	var step_result = stmt.step()
	if step_result != 101:
		_log("Failed to update task: " + db.errmsg())
	else:
		_log("Marked task " + str(id) + " as done")
	stmt.finalize()

func update_priority(db: SQLite3Database, id: int, priority: int):
	var stmt = db.prepare("UPDATE tasks SET priority = ? WHERE id = ?")
	if stmt == null:
		_log("Failed to prepare update priority statement")
		return
	stmt.bind_int(1, priority)
	stmt.bind_int(2, id)
	var step_result = stmt.step()
	if step_result != 101:
		_log("Failed to update priority: " + db.errmsg())
	else:
		_log("Updated priority of task " + str(id) + " to " + str(priority))
	stmt.finalize()

func search_tasks(db: SQLite3Database, keyword: String):
	_log("Searching tasks with keyword: " + keyword)
	var stmt = db.prepare("SELECT id, title FROM tasks WHERE title LIKE ?")
	if stmt == null:
		_log("Failed to prepare search statement")
		return
	stmt.bind_text(1, "%" + keyword + "%")
	while stmt.step() == 100:  # SQLITE_ROW
		var title = stmt.column_text(1)
		_log("Found: " + title)
	stmt.finalize()

func test_transaction(db: SQLite3Database):
	_log("Testing transactions")
	db.exec("BEGIN")
	var _temp_id = insert_task(db, "Transactional task", "This should be rolled back", 1)
	list_tasks(db)
	db.exec("ROLLBACK")
	_log("After rollback:")
	list_tasks(db)
	db.exec("BEGIN")
	var _commit_id = insert_task(db, "Committed task", "This should stay", 1)
	db.exec("COMMIT")
	_log("After commit:")
	list_tasks(db)

func test_blob(db: SQLite3Database):
	_log("Testing BLOB operations")
	var blob_data = PackedByteArray([72, 101, 108, 108, 111, 32, 87, 111, 114, 108, 100])  # "Hello World" in bytes
	var stmt = db.prepare("INSERT INTO tasks (title, description) VALUES (?, ?)")
	stmt.bind_text(1, "Blob test")
	stmt.bind_blob(2, blob_data)
	stmt.step()
	stmt.finalize()
	_log("Inserted blob data")

	var query_stmt = db.prepare("SELECT description FROM tasks WHERE title = ?")
	query_stmt.bind_text(1, "Blob test")
	if query_stmt.step() == 100:
		var retrieved_blob = query_stmt.column_blob(0)
		_log("Retrieved blob: " + str(retrieved_blob) + " (as string: " + retrieved_blob.get_string_from_utf8() + ")")
	query_stmt.finalize()

func test_backup(db: SQLite3Database):
	_log("Testing backup operations")
	var backup_db = SQLite3Database.open("backup.db")
	if backup_db == null:
		_log("Failed to open backup database")
		return
	var backup = db.backup_init("main", backup_db, "main")
	if backup == null:
		_log("Failed to initialize backup")
		backup_db.close()
		return
	# Perform backup steps (simplified)
	while backup.step(100) == 100:
		pass
	backup.finish()
	_log("Backup completed")
	backup_db.close()
	# Clean up
	DirAccess.remove_absolute("backup.db")

func test_wal(db: SQLite3Database):
	_log("Testing WAL mode")
	var wal_result = db.exec("PRAGMA journal_mode = WAL")
	if wal_result == 0:
		_log("WAL mode enabled")
	else:
		_log("Failed to enable WAL mode")

func log_database_status(db: SQLite3Database):
	_log("Database status:")
	var status = db.db_status(0)  # SQLITE_DBSTATUS_LOOKASIDE_USED
	_log("Lookaside used: " + str(status[1]) + "/" + str(status[2]))

func log(message: String):
	print(message)
	if log_label:
		log_label.text += message + "\n"
