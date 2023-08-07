#include "DatabaseClients.h"


DatabaseClients::DatabaseClients(const std::string& username, const std::string& db_name, const std::string& password) :
		m_username(username), m_db_name(db_name), m_password(password) {
	m_c = std::make_unique<pqxx::connection>(
		"host=127.0.0.1 "
		"port=5432 "
		"dbname=" + m_db_name + " "
		"user=" + m_username + " "
		"password=" + m_password);

	m_c->prepare("add_Client", "INSERT INTO Client(name, surname, email) VALUES ($1, $2, $3)");
	m_c->prepare("add_Phone", "INSERT INTO Phone(number) VALUES ($1)");
	m_c->prepare("add_ClientPhone", "INSERT INTO ClientPhone(client_id, phone_id) VALUES ($1, $2)");
	//m_c->prepare("change_fname", "update dbusers set fname = $1 where userid = $2;");
	//m_c->prepare("change_lname", "update dbusers set lname = $1 where userid = $2;");
	//m_c->prepare("change_email", "update dbusers set email = $1 where userid = $2;");
}

void DatabaseClients::make_DB() const {
	exec("CREATE TABLE IF NOT EXISTS Client ("
		"id SERIAL PRIMARY KEY, "
		"name text NOT NULL, "
		"surname text NOT NULL, "
		"email text UNIQUE NOT null)");

	exec("CREATE TABLE IF NOT EXISTS Phone ("
		"id SERIAL PRIMARY KEY, "
		"number text UNIQUE NOT NULL)");

	exec("CREATE TABLE IF NOT EXISTS ClientPhone ("
		"client_id INTEGER REFERENCES Client(id), "
		"phone_id INTEGER REFERENCES Phone(id), "
		"CONSTRAINT pk_ClientPhone PRIMARY KEY(client_id, phone_id))");
}

void DatabaseClients::print_DB() const {
	pqxx::work tx(*m_c);
	int i = 0;
	for (const auto& [id, name, surname, email] : tx.query<int, std::string, std::string, std::string>("SELECT id, name, surname, email FROM Client ORDER BY id ASC;")) {
		set_cursor(0, 7 + i);
		std::cout << id;
		set_cursor(4, 7 + i);
		std::cout << name;
		set_cursor(20, 7 + i);
		std::cout << surname;
		set_cursor(40, 7 + i);
		std::cout << email;
		for (const auto& [phone] : tx.query<std::string>("SELECT p.number FROM Phone p LEFT JOIN ClientPhone cp ON p.id = cp.phone_id LEFT JOIN Client c ON cp.client_id = c.id WHERE email = '" + email + "'")) {
			set_cursor(70, 7 + i);
			std::cout << phone;
			i++;
		}
		i++;
	}
}

void DatabaseClients::addClient(const std::string& name, const std::string& surname, const std::string& email, const std::vector<std::string>& phone) const {
	//pqxx::work tx(*m_c);
	//tx.exec("INSERT INTO Client(name, surname, email) VALUES ('" + name + "', '" + surname + "', '" + email + "')");
	//tx.commit();
	exec_prepared_add_Client(name, surname, email);

	if (!phone.empty()) {
		int id_client = get_id_Client(email);
		/*pqxx::work tx2(*m_c);
		for (const auto& it : phone) {
			tx2.exec("INSERT INTO Phone(number) VALUES ('" + it + "')");
		}
		tx2.commit();*/
		for (const auto& it : phone) {
			exec_prepared_add_Phone(it);
			int id_phone = get_id_Phone(it);
			exec_prepared_add_ClientPhone(std::to_string(id_client), std::to_string(id_phone));
		}

		/*pqxx::work tx3(*m_c);
		int id_client = tx3.query_value<int>("SELECT id FROM Client WHERE email = '" + email + "'");
		for (size_t i = 0; i < phone.size(); ++i) {
			int id_phone = tx3.query_value<int>("SELECT id FROM Phone WHERE number = '" + phone[i] + "'");
			tx3.exec("INSERT INTO ClientPhone(client_id, phone_id) VALUES ('" + std::to_string(id_client) + "', '" + std::to_string(id_phone) + "')");
		}
		tx3.commit();*/

		/*id_client = get_id_Client(email);
		for (size_t i = 0; i < phone.size(); ++i) {
			id_phone = get_id_Phone(phone[i]);
			exec_prepared_add_ClientPhone(std::to_string(id_client), std::to_string(id_phone));
		}*/
	}
}

void DatabaseClients::addClientPhone(const int id_client, const std::vector<std::string>& phone) const {
	for (const auto& it : phone) {
		exec_prepared_add_Phone(it);
		int id_phone = get_id_Phone(it);
		exec_prepared_add_ClientPhone(std::to_string(id_client), std::to_string(id_phone));
	}
}

int DatabaseClients::size() const {
	return get_number_of_Clients();
}

void DatabaseClients::set_cursor(const int x, const int y) const {
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { static_cast<short>(x), static_cast<short>(y) });
}

void DatabaseClients::exec(std::string str) const {
	pqxx::work tx(*m_c);
	tx.exec(str);
	tx.commit();
}

void DatabaseClients::exec_prepared_add_Client(const std::string& name, const std::string& surname, const std::string& email) const {
	pqxx::work tx(*m_c);
	tx.exec_prepared("add_Client", name, surname, email);
	tx.commit();
}

void DatabaseClients::exec_prepared_add_Phone(const std::string& phone) const {
	pqxx::work tx(*m_c);
	tx.exec_prepared("add_Phone", phone);
	tx.commit();
}

void DatabaseClients::exec_prepared_add_ClientPhone(const std::string& id_client, const std::string& id_phone) const {
	pqxx::work tx(*m_c);
	tx.exec_prepared("add_ClientPhone", id_client, id_phone);
	tx.commit();
}

int DatabaseClients::get_number_of_Clients() const {
	pqxx::work tx(*m_c);
	int number_of_clients = tx.query_value<int>("SELECT count(id) FROM Client");
	tx.commit();

	return number_of_clients;
}

int DatabaseClients::get_id_Client(const std::string& email) const {
	pqxx::work tx(*m_c);
	int id_client = tx.query_value<int>("SELECT id FROM Client WHERE email = '" + email + "'");
	tx.commit();

	return id_client;
}

int DatabaseClients::get_id_Phone(const std::string& phone) const {
	pqxx::work tx(*m_c);
	int id_phone = tx.query_value<int>("SELECT id FROM Phone WHERE number = '" + phone + "'");
	tx.commit();

	return id_phone;
}
