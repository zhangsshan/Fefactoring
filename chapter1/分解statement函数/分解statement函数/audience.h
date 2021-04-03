#pragma once

#include<QString>
#include<vector>
#include<map>
#include<iostream>

using namespace std;

enum class PlayType : int
{
	TRAGEDY = 0,//±¯¾ç
	COMEDY = 1,//Ï²¾ç
	UNKNOWN = 2,//Ï²¾ç
};

enum class ErrType : int
{
	Error_Bad_File = 0
};

struct Play
{
	Play()
		:m_type(PlayType::TRAGEDY)
	{
	}
	Play(QString strName, PlayType type)
		:m_strName(strName), m_type(type)
	{
	}
	QString m_strName;
	PlayType m_type;
};

struct Performances
{
	Performances(QString strPlayID, int nAudience)
		:m_strPlayID(strPlayID), m_nAudience(nAudience)
	{
	}
	QString m_strPlayID;
	int m_nAudience;
};

struct Invoice
{
	Invoice(QString strCustomer, std::vector<Performances> performances)
		:m_strCustomer(strCustomer), m_performances(performances)
	{}
	QString m_strCustomer;
	std::vector<Performances> m_performances;
};
void statement(Invoice invoice, map<QString, Play> plays);
map<QString, Play> parseJsonFileToPlays(QString strJsonFilePath);
Invoice parseJsonFileToInvoices(QString strJsonFilePath);