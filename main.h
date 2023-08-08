#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <Windows.h>
#include <stringapiset.h>

#include "DatabaseClients.h"


enum class e_main_menu {
	exit,
	getDatabase,
	addClient,
	deleteClient,
	addPhone,
	deletePhone,
	changeClient,
	findClient
};


void main_menu(const std::unique_ptr<DatabaseClients>& db_clnt);
void get_database(const std::unique_ptr<DatabaseClients>& db_clnt);
void menu_add_client(const std::unique_ptr<DatabaseClients>& db_clnt);
void menu_delete_client(const std::unique_ptr<DatabaseClients>& db_clnt);
void menu_add_phone(const std::unique_ptr<DatabaseClients>& db_clnt);
void menu_delete_phone(const std::unique_ptr<DatabaseClients>& db_clnt);
void menu_change_client(const std::unique_ptr<DatabaseClients>& db_clnt);
void menu_find_client(const std::unique_ptr<DatabaseClients>& db_clnt);

std::string cp1251_to_utf8(const char* str);

void print_title();
void show_database(const std::unique_ptr<std::vector<DatabaseClients::Client>>& clients, const int x, const int y);
void wait_user();
void set_cursor(const int x, const int y);
