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
	m_c->prepare("change_name", "UPDATE Client SET name = $1 WHERE id = $2;");
	m_c->prepare("change_surname", "UPDATE Client SET surname = $1 WHERE id = $2;");
	m_c->prepare("change_email", "UPDATE Client SET email = $1 WHERE id = $2;");
	m_c->prepare("delete_Client", "DELETE FROM Client WHERE id = $1;");
	m_c->prepare("delete_Phone", "DELETE FROM Phone WHERE id = $1;");
	m_c->prepare("delete_ClientPhone_one_param", "DELETE FROM ClientPhone WHERE client_id = $1;");
	m_c->prepare("delete_ClientPhone_two_param", "DELETE FROM ClientPhone WHERE client_id = $1 AND phone_id = $2;");
}

void DatabaseClients::make_DB() const {
	exec("CREATE TABLE IF NOT EXISTS Client ("
		"id SERIAL PRIMARY KEY, "
		"name text NOT NULL, "
		"surname text NOT NULL, "
		"email text UNIQUE NOT NULL)");

	exec("CREATE TABLE IF NOT EXISTS Phone ("
		"id SERIAL PRIMARY KEY, "
		"number text UNIQUE NOT NULL)");

	exec("CREATE TABLE IF NOT EXISTS ClientPhone ("
		"client_id INTEGER REFERENCES Client(id), "
		"phone_id INTEGER REFERENCES Phone(id), "
		"CONSTRAINT pk_ClientPhone PRIMARY KEY(client_id, phone_id))");
}

std::vector<DatabaseClients::Client> DatabaseClients::get_DB() const {
	std::vector<DatabaseClients::Client> clients;
	pqxx::work tx(*m_c);

	for (const auto& [id, name, surname, email] : tx.query<int, std::string, std::string, std::string>(
		"SELECT id, name, surname, email FROM Client "
		"ORDER BY id ASC")) {
		std::vector<std::string> phones;
		for (const auto& [phone] : tx.query<std::string>(
			"SELECT p.number FROM Phone p "
			"LEFT JOIN ClientPhone cp ON p.id = cp.phone_id "
			"LEFT JOIN Client c ON cp.client_id = c.id "
			"WHERE email = '" + email + "'")) {
			phones.emplace_back(phone);
		}

		clients.emplace_back(id, name, surname, email, std::move(phones));
	}
	tx.abort();

	return clients;
}

void DatabaseClients::addClient(const std::string& name, const std::string& surname, const std::string& email, const std::vector<std::string>& phone) const {
	exec_prepared_add_Client(name, surname, email);

	if (!phone.empty()) {
		int id_client = get_id_Client(email);
		addClientPhone(id_client, phone);
	}
}

void DatabaseClients::deleteClient(const int id_client) const {
	std::vector<int> id_phones;
	pqxx::work tx(*m_c);
	for (const auto& [id_phone] : tx.query<int>(
		"SELECT p.id FROM Phone p "
		"LEFT JOIN ClientPhone cp ON p.id = cp.phone_id "
		"WHERE cp.client_id = " + std::to_string(id_client))) {
		id_phones.emplace_back(id_phone);
	}
	tx.exec_prepared("delete_ClientPhone_one_param", id_client);
	tx.exec_prepared("delete_Client", id_client);
	for (const auto& it : id_phones) {
		tx.exec_prepared("delete_Phone", it);
	}
	tx.commit();
}

void DatabaseClients::addClientPhone(const int id_client, const std::vector<std::string>& phone) const {
	for (const auto& it : phone) {
		exec_prepared_add_Phone(it);
		int id_phone = get_id_Phone(it);
		exec_prepared_add_ClientPhone(std::to_string(id_client), std::to_string(id_phone));
	}
}

void DatabaseClients::deleteClientPhone(const int id_client, const int id_phone) const {
	pqxx::work tx(*m_c);
	tx.exec_prepared("delete_ClientPhone_two_param", id_client, id_phone);
	tx.exec_prepared("delete_Phone", id_phone);
	tx.commit();
}

void DatabaseClients::changeClient(const int id_client, const std::string& new_data, const e_change e_ch) const {
	if (e_ch == e_change::name) {
		pqxx::work tx(*m_c);
		tx.exec_prepared("change_name", new_data, id_client);
		tx.commit();
	}
	else if (e_ch == e_change::surname) {
		pqxx::work tx(*m_c);
		tx.exec_prepared("change_surname", new_data, id_client);
		tx.commit();
	}
	else if (e_ch == e_change::email) {
		pqxx::work tx(*m_c);
		tx.exec_prepared("change_email", new_data, id_client);
		tx.commit();
	}
}

std::vector<DatabaseClients::Client> DatabaseClients::findClient(const std::string& search_str, const e_change e_ch) const {
	std::vector<DatabaseClients::Client> clients;
	pqxx::work tx(*m_c);

	if (e_ch == e_change::name) {
		for (const auto& [id, name, surname, email] : tx.query<int, std::string, std::string, std::string>(
			"SELECT id, name, surname, email FROM Client "
			"WHERE name = '" + search_str + "'"
			"ORDER BY id ASC")) {
			std::vector<std::string> phones;
			for (const auto& [phone] : tx.query<std::string>(
				"SELECT p.number FROM Phone p "
				"LEFT JOIN ClientPhone cp ON p.id = cp.phone_id "
				"LEFT JOIN Client c ON cp.client_id = c.id "
				"WHERE email = '" + email + "'")) {
				phones.emplace_back(phone);
			}
			clients.emplace_back(id, name, surname, email, std::move(phones));
		}
	}
	else if (e_ch == e_change::surname) {
		for (const auto& [id, name, surname, email] : tx.query<int, std::string, std::string, std::string>(
			"SELECT id, name, surname, email FROM Client "
			"WHERE surname = '" + search_str + "'"
			"ORDER BY id ASC")) {
			std::vector<std::string> phones;
			for (const auto& [phone] : tx.query<std::string>(
				"SELECT p.number FROM Phone p "
				"LEFT JOIN ClientPhone cp ON p.id = cp.phone_id "
				"LEFT JOIN Client c ON cp.client_id = c.id "
				"WHERE email = '" + email + "'")) {
				phones.emplace_back(phone);
			}
			clients.emplace_back(id, name, surname, email, std::move(phones));
		}
	}
	else if (e_ch == e_change::email) {
		for (const auto& [id, name, surname, email] : tx.query<int, std::string, std::string, std::string>(
			"SELECT id, name, surname, email FROM Client "
			"WHERE email = '" + search_str + "'"
			"ORDER BY id ASC")) {
			std::vector<std::string> phones;
			for (const auto& [phone] : tx.query<std::string>(
				"SELECT p.number FROM Phone p "
				"LEFT JOIN ClientPhone cp ON p.id = cp.phone_id "
				"LEFT JOIN Client c ON cp.client_id = c.id "
				"WHERE email = '" + email + "'")) {
				phones.emplace_back(phone);
			}
			clients.emplace_back(id, name, surname, email, std::move(phones));
		}
	}
	else if (e_ch == e_change::phone) {
		for (const auto& [id, name, surname, email] : tx.query<int, std::string, std::string, std::string>(
			"SELECT c.id, c.name, c.surname, c.email FROM Client c "
			"LEFT JOIN ClientPhone cp ON c.id = cp.client_id "
			"LEFT JOIN Phone p ON cp.phone_id = p.id "
			"WHERE p.number = '" + search_str + "'"
			"ORDER BY c.id ASC")) {
			std::vector<std::string> phones;
			for (const auto& [phone] : tx.query<std::string>(
				"SELECT p.number FROM Phone p "
				"LEFT JOIN ClientPhone cp ON p.id = cp.phone_id "
				"LEFT JOIN Client c ON cp.client_id = c.id "
				"WHERE email = '" + email + "'")) {
				phones.emplace_back(phone);
			}
			clients.emplace_back(id, name, surname, email, std::move(phones));
		}
	}
	tx.abort();

	return clients;
}

int DatabaseClients::size() const {
	return get_number_of_Clients();
}

std::string DatabaseClients::get_name(const int id_client) const {
	pqxx::work tx(*m_c);
	std::string name = tx.query_value<std::string>("SELECT name FROM Client WHERE id = '" + std::to_string(id_client) + "'");
	tx.abort();

	return name;
}

std::string DatabaseClients::get_surname(const int id_client) const {
	pqxx::work tx(*m_c);
	std::string surname = tx.query_value<std::string>("SELECT surname FROM Client WHERE id = '" + std::to_string(id_client) + "'");
	tx.abort();

	return surname;
}

std::string DatabaseClients::get_email(const int id_client) const {
	pqxx::work tx(*m_c);
	std::string email = tx.query_value<std::string>("SELECT email FROM Client WHERE id = '" + std::to_string(id_client) + "'");
	tx.abort();

	return email;
}

std::map<int, std::string> DatabaseClients::get_phone(const int id_client) const {
	std::map<int, std::string> map_phones;
	pqxx::work tx(*m_c);

	for (const auto& [id, phone] : tx.query<int, std::string>(
		"SELECT p.id, p.number FROM Phone p "
		"LEFT JOIN ClientPhone cp ON p.id = cp.phone_id "
		"LEFT JOIN Client c ON cp.client_id = c.id "
		"WHERE c.id = " + std::to_string(id_client))) {
		map_phones.insert(std::make_pair(id, phone));
	}
	tx.abort();

	return map_phones;
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
	tx.abort();

	return number_of_clients;
}

int DatabaseClients::get_id_Client(const std::string& email) const {
	pqxx::work tx(*m_c);
	int id_client = tx.query_value<int>("SELECT id FROM Client WHERE email = '" + email + "'");
	tx.abort();

	return id_client;
}

int DatabaseClients::get_id_Phone(const std::string& phone) const {
	pqxx::work tx(*m_c);
	int id_phone = tx.query_value<int>("SELECT id FROM Phone WHERE number = '" + phone + "'");
	tx.abort();

	return id_phone;
}
