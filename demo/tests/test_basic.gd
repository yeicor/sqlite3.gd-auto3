extends RefCounted

func run_test(db: SQLite3Database, log_func: Callable):
	log_func.call("Starting Basic Database Operations Test", "TEST_START")

	# Enable foreign keys
	var result = db.exec("PRAGMA foreign_keys = ON")
	if result == SQLite3Database.SQLITE_OK:
		log_func.call("Foreign keys enabled", "SUCCESS")
	else:
		log_func.call("Failed to enable foreign keys: " + db.errmsg(), "ERROR")
		return

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
	result = db.exec(create_sql)
	if result == SQLite3Database.SQLITE_OK:
		log_func.call("Table 'tasks' created", "SUCCESS")
	else:
		log_func.call("Failed to create table: " + db.errmsg(), "ERROR")
		return

	# Create categories table
	var create_cat_sql = """
	CREATE TABLE categories (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		name TEXT UNIQUE NOT NULL
	)
	"""
	db.exec(create_cat_sql)
	log_func.call("Table 'categories' created", "SUCCESS")

	# Create junction table
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
	log_func.call("Junction table 'task_categories' created", "SUCCESS")

	# Create indexes
	db.exec("CREATE INDEX idx_tasks_done ON tasks(done)")
	db.exec("CREATE INDEX idx_tasks_title ON tasks(title)")
	log_func.call("Indexes created", "SUCCESS")

	# Insert categories
	var cat1 = insert_category(db, "Work", log_func)
	var cat2 = insert_category(db, "Personal", log_func)
	var cat3 = insert_category(db, "Shopping", log_func)

	# Insert tasks
	var task1 = insert_task(db, "Buy groceries", "Milk, bread, eggs", 2, log_func)
	var task2 = insert_task(db, "Finish project", "Complete the SQLite demo", 5, log_func)
	var task3 = insert_task(db, "Exercise", "Go for a run", 3, log_func)

	# Assign categories
	assign_category(db, task1, cat3, log_func)  # Shopping
	assign_category(db, task2, cat1, log_func)  # Work
	assign_category(db, task3, cat2, log_func)  # Personal

	# List tasks
	list_tasks(db, log_func)

	# Mark task as done
	mark_done(db, task1, log_func)

	# List again
	list_tasks(db, log_func)

	log_func.call("Basic Database Operations Test completed", "TEST_END")

func insert_category(db: SQLite3Database, name: String, log_func: Callable) -> int:
	var stmt = db.prepare("INSERT INTO categories (name) VALUES (?)")
	if stmt == null:
		log_func.call("Failed to prepare insert category statement", "ERROR")
		return -1
	stmt.bind_text(1, name)
	var step_result = stmt.step()
	if step_result != SQLite3Database.SQLITE_DONE:
		log_func.call("Failed to insert category: " + db.errmsg(), "ERROR")
		stmt.finalize()
		return -1
	var id = db.last_insert_rowid()
	log_func.call("Inserted category: " + name + " (ID: " + str(id) + ")", "INFO")
	stmt.finalize()
	return id

func insert_task(db: SQLite3Database, title: String, desc: String, priority: int, log_func: Callable) -> int:
	var stmt = db.prepare("INSERT INTO tasks (title, description, priority) VALUES (?, ?, ?)")
	if stmt == null:
		log_func.call("Failed to prepare insert task statement", "ERROR")
		return -1
	stmt.bind_text(1, title)
	stmt.bind_text(2, desc)
	stmt.bind_int(3, priority)
	var step_result = stmt.step()
	if step_result != SQLite3Database.SQLITE_DONE:
		log_func.call("Failed to insert task: " + db.errmsg(), "ERROR")
		stmt.finalize()
		return -1
	var id = db.last_insert_rowid()
	log_func.call("Inserted task: " + title + " (ID: " + str(id) + ")", "INFO")
	stmt.finalize()
	return id

func assign_category(db: SQLite3Database, task_id: int, cat_id: int, log_func: Callable):
	var stmt = db.prepare("INSERT INTO task_categories (task_id, category_id) VALUES (?, ?)")
	if stmt == null:
		log_func.call("Failed to prepare assign category statement", "ERROR")
		return
	stmt.bind_int(1, task_id)
	stmt.bind_int(2, cat_id)
	var step_result = stmt.step()
	if step_result != SQLite3Database.SQLITE_DONE:
		log_func.call("Failed to assign category: " + db.errmsg(), "ERROR")
	else:
		log_func.call("Assigned category " + str(cat_id) + " to task " + str(task_id), "INFO")
	stmt.finalize()

func list_tasks(db: SQLite3Database, log_func: Callable):
	log_func.call("Listing all tasks:", "INFO")
	var result_set = db.query("SELECT t.id, t.title, t.priority, t.done, GROUP_CONCAT(c.name, ', ') as categories FROM tasks t LEFT JOIN task_categories tc ON t.id = tc.task_id LEFT JOIN categories c ON tc.category_id = c.id GROUP BY t.id")
	while result_set.next():
		var row = result_set.current_row()
		var status = "Done" if row["done"] == 1 else "Pending"
		var cats = row["categories"] if row["categories"] != null else "None"
		log_func.call("ID: %d, Title: %s, Priority: %d, Status: %s, Categories: %s" % [row["id"], row["title"], row["priority"], status, cats], "DATA")
	result_set.close()

func mark_done(db: SQLite3Database, id: int, log_func: Callable):
	var stmt = db.prepare("UPDATE tasks SET done = 1 WHERE id = ?")
	if stmt == null:
		log_func.call("Failed to prepare update statement", "ERROR")
		return
	stmt.bind_int(1, id)
	var step_result = stmt.step()
	if step_result != SQLite3Database.SQLITE_DONE:
		log_func.call("Failed to update task: " + db.errmsg(), "ERROR")
	else:
		log_func.call("Marked task " + str(id) + " as done", "INFO")
	stmt.finalize()
