#include"audience.h"
#include<iostream>
#include<algorithm>
#include<fstream>
#include<sstream>

#include<QJsonParseError>
#include<QJsonDocument>
#include<QJsonArray>
#include<QJsonObject>
#include<QJsonValue>

#include<QFile>

using namespace std;

static const std::string strErrorType = "ErrType";

std::map<std::string, PlayType> type2Name = { {"tragedy",PlayType::TRAGEDY}, {"comedy", PlayType::COMEDY} };

void statement(Invoice invoice, map<QString, Play> plays)
{
	int nTotalAmount = 0;
	int nVolumeCredits = 0;
	
	QString strResult = QString("State for %1\n").arg(invoice.m_strCustomer);
	//遍历账单，打印账单详情
	for (auto ite = invoice.m_performances.begin();ite != invoice.m_performances.end();++ite)
	{
		Play aPlay = plays[ite->m_strPlayID];
		//计算每个剧的积分
		int nAmount = 0;

		switch (aPlay.m_type)
		{
		case PlayType::TRAGEDY:
			nAmount = 40000;
			if (ite->m_nAudience > 30)
			{
				nAmount += 1000 * (ite->m_nAudience - 30);
			}
			break;
		case PlayType::COMEDY:
			nAmount = 30000;
			if (ite->m_nAudience > 20)
			{
				nAmount += 10000 + 500 * (ite->m_nAudience - 20);
			}
			nAmount += 300 * (ite->m_nAudience);
			break;
		default:
			throw std::runtime_error(strErrorType);
		}
		//观众量积分
		nVolumeCredits += ite->m_nAudience - 30 > 0? ite->m_nAudience - 30:0;
		if (aPlay.m_type == PlayType::COMEDY)
		{
			nVolumeCredits += ite->m_nAudience / 5;
		}
		strResult += QString("%1:%2 (%3 seats)\n").arg(aPlay.m_strName).arg(nAmount / 100)
			.arg(ite->m_nAudience);
		nTotalAmount += nAmount;
	}
	strResult += QString("Amount owed is %1\n").arg(nTotalAmount/100);
	strResult += QString("You earned %1 credits\n").arg(nVolumeCredits);

	//写文件
	std::string strFilePath("./statement.txt");
	ofstream fout(strFilePath, std::ios::out);
	if (fout.good())
	{
		//ostringstream ostream(strResult.toStdString());
		fout << strResult.toStdString();
	}
	fout.close();
}

map<QString, Play> parseJsonFileToPlays(QString strJsonFilePath)
{
	QFile jsonFile(strJsonFilePath);
	jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QString strJsonExpr = jsonFile.readAll();
	jsonFile.close();

	map<QString, Play> result;
	//解析json表达式
	QJsonParseError jsonParseError;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(strJsonExpr.toUtf8(), &jsonParseError);
	if ((jsonParseError.error != QJsonParseError::NoError))
	{
		throw ErrType::Error_Bad_File;
	}
	else
	{
		QJsonObject jsonObject = jsonDocument.object();
		for (auto ite = jsonObject.begin();ite != jsonObject.end();++ite)
		{
			QJsonObject jsonSubObject = ite.value().toObject();
			QString strName = jsonSubObject.value("name").toString();
			QString strType = jsonSubObject.value("type").toString();
			PlayType aPlayType;
			auto iteType = type2Name.find(strType.toStdString());
			if (iteType != type2Name.end())
			{
				aPlayType= iteType->second;
			}
			else
			{
				aPlayType = PlayType::UNKNOWN;
			}			
			result.insert(std::pair<QString, Play>(ite.key(), Play(strName, aPlayType)));
		}
	}
	return result;
}

Invoice parseJsonFileToInvoices(QString strJsonFilePath)
{
	QFile jsonFile(strJsonFilePath);
	jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QString strJsonExpr = jsonFile.readAll();
	jsonFile.close();

	std::vector<Performances> performances;
	QString strCustomer;
	
	//解析json表达式
	QJsonParseError jsonParseError;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(strJsonExpr.toUtf8(), &jsonParseError);
	if ((jsonParseError.error != QJsonParseError::NoError))
	{
		throw ErrType::Error_Bad_File;
	}
	else
	{
		if (jsonDocument.isArray())
		{
			for (int i = 0; i < jsonDocument.array().count(); i++)
			{
				QJsonObject jsonObject = jsonDocument.array().at(i).toObject();
				//用户自定义内容
				if (jsonObject.contains("customer"))
				{
					strCustomer = jsonObject.value("customer").toString();
				}
				if (jsonObject.contains("performances"))
				{
					QJsonArray jsonArray = jsonObject.value("performances").toArray();					
					for (int nIdx = 0; nIdx < jsonArray.count();++nIdx)
					{
						QJsonObject jsonSubObject = jsonArray.at(nIdx).toObject();
						QString strPlayID = jsonSubObject.value("playID").toString();
						int nAudience = jsonSubObject.value("audience").toInt();
						performances.emplace_back(Performances(strPlayID, nAudience));
					}
				}
			}
		}		
	}
	return Invoice(strCustomer, performances);
}


int main()
{
	try
	{
		Invoice invoice = parseJsonFileToInvoices(QString::fromLocal8Bit("G:\\重构\\分解statement函数\\invoices.json"));
		map<QString, Play> plays = parseJsonFileToPlays(QString::fromLocal8Bit("G:\\重构\\分解statement函数\\plays.json"));
		statement(invoice, plays);
	}
	catch (...)
	{

	}
	
	return 0;
}

