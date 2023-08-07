#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <Windows.h>
#include <stringapiset.h>

#include "DatabaseClients.h"


enum class e_main_menu {
	exit,
	printDatabase,
	addClient,
	addPhone,
	changeClient
};


void main_menu(const std::unique_ptr<DatabaseClients>& db_clnt);
void print_DB(const std::unique_ptr<DatabaseClients>& db_clnt);
void menu_add_client(const std::unique_ptr<DatabaseClients>& db_clnt);
void menu_add_phone(const std::unique_ptr<DatabaseClients>& db_clnt);
void menu_change_client(const std::unique_ptr<DatabaseClients>& db_clnt);

std::string cp1251_to_utf8(const char* str);

void print_title();
void wait_user();
