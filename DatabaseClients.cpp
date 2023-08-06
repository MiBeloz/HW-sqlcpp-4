#include "DatabaseClients.h"


DatabaseClients::DatabaseClients(std::string& username, std::string& db_name, std::string& password) : 
		m_username(username), m_db_name(db_name), m_password(password) {
	m_c = std::make_unique<pqxx::connection>(
		"host=127.0.0.1 "
		"port=5432 "
		"dbname=" + m_db_name + " "
		"user=" + m_username + " "
		"password=" + m_password);
}

void DatabaseClients::make_DB() {
	pqxx::work tx(*m_c);
	tx.exec("CREATE TABLE IF NOT EXISTS Client ("
		"id SERIAL PRIMARY KEY, "
		"name text NOT NULL, "
		"surname text NOT NULL, "
		"email text UNIQUE NOT null)");
	tx.exec("CREATE TABLE IF NOT EXISTS Phone ("
		"id SERIAL PRIMARY KEY, "
		"number text UNIQUE NOT NULL)");
	tx.exec("CREATE TABLE IF NOT EXISTS ClientPhone ("
		"client_id INTEGER REFERENCES Client(id), "
		"phone_id INTEGER REFERENCES Phone(id), "
		"CONSTRAINT pk_ClientPhone PRIMARY KEY(client_id, phone_id))");
	tx.commit();
}

void DatabaseClients::addClient(std::string& name, std::string& surname, std::string& email, std::vector<std::string>& phone) {
	pqxx::work tx(*m_c);
	tx.exec("INSERT INTO Client(name, surname, email) VALUES ('" + name + "', '" + surname + "', '" + email + "')");
	tx.commit();
	if (!phone.empty()) {
		pqxx::work tx2(*m_c);
		for (const auto& it : phone) {
			tx2.exec("INSERT INTO Phone(number) VALUES ('" + it + "')");
		}
		tx2.commit();

		pqxx::work tx3(*m_c);
		int id_client = tx3.query_value<int>("SELECT id FROM Client WHERE email = '" + email + "'");
		for (size_t i = 0; i < phone.size(); ++i) {
			int id_phone = tx3.query_value<int>("SELECT id FROM Phone WHERE number = '" + phone[i] + "'");
			tx3.exec("INSERT INTO ClientPhone(client_id, phone_id) VALUES ('" + std::to_string(id_client) + "', '" + std::to_string(id_phone) + "')");
		}
		tx3.commit();
	}
}

