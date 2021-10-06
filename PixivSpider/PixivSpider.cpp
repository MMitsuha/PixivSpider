#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <regex>
#include <boost/log/trivial.hpp>
#include <utf8h/utf8.h>
#include <json/json.h>
#include <mutex>
#include <vector>
#include <thread>

std::wstring utf82uni(const std::string& utf8_string)
{
	auto wide_size = MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, NULL, 0);
	if (wide_size == ERROR_NO_UNICODE_TRANSLATION)
		throw std::exception("Invalid UTF-8 sequence.");
	if (wide_size == 0)
		throw std::exception("Error in conversion.");
	std::wstring uni_string;
	uni_string.resize(wide_size - 1ull);
	MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, &uni_string[0], wide_size - 1);

	return uni_string;
}

std::string uni2utf8(const std::wstring& uni_string)
{
	auto multi_size = WideCharToMultiByte(CP_UTF8, 0, uni_string.c_str(), -1, NULL, 0, NULL, NULL);
	if (multi_size == ERROR_NO_UNICODE_TRANSLATION)
		throw std::exception("Invalid UTF-8 sequence.");
	if (multi_size == 0)
		throw std::exception("Error in conversion.");
	std::string utf8_string;
	utf8_string.resize(multi_size - 1ull);
	WideCharToMultiByte(CP_UTF8, 0, uni_string.c_str(), -1, &utf8_string[0], multi_size - 1, NULL, NULL);

	return utf8_string;
}

std::mutex g_locker;
size_t g_count = 1;

bool getSinglePicture(std::string url)
{
	try
	{
		curlpp::Cleanup cleanup;

		std::ostringstream utf8_response_html;
		curlpp::Easy request_html;
		request_html.setOpt(cURLpp::Options::Proxy("127.0.0.1"));
		request_html.setOpt(cURLpp::Options::ProxyPort(10809));
		request_html.setOpt(cURLpp::Options::HttpProxyTunnel(true));
		request_html.setOpt(cURLpp::Options::Url(url));
		request_html.setOpt(cURLpp::Options::WriteStream(&utf8_response_html));
		request_html.setOpt(cURLpp::Options::UserAgent("Mozilla/5.0"));
		request_html.setOpt(cURLpp::Options::Referer("https://www.pixiv.net/ranking.php?mode=daily&content=illust"));

		request_html.perform();

		std::string utf8_response_html_string = utf8_response_html.str();

		auto utf8_search_file_name_result = utf8str(utf8_response_html_string.c_str(), "\"illustTitle\":\"");
		auto utf8_search_original_url_result = utf8str(utf8_response_html_string.c_str(), "\"original\":\"");
		if (utf8_search_file_name_result && utf8_search_original_url_result)
		{
			std::string utf8_file_name;
			std::string utf8_original_url;
			g_locker.lock();
			utf8_file_name += std::to_string(g_count);
			++g_count;
			g_locker.unlock();
			utf8_file_name += "_";

			char* utf8_file_name_end = (char*)utf8_search_file_name_result + sizeof("\"illustTitle\":\"") - 1;
			for (; *utf8_file_name_end != 0; utf8_file_name_end++)
			{
				if (*utf8_file_name_end == '"')
					break;

				/*if (*utf8_file_name_end == '\\' ||
					*utf8_file_name_end == '/' ||
					*utf8_file_name_end == ':' ||
					*utf8_file_name_end == '*' ||
					*utf8_file_name_end == '?' ||
					*utf8_file_name_end == '>' ||
					*utf8_file_name_end == '<' ||
					*utf8_file_name_end == '|')
					continue;*/

				utf8_file_name += *utf8_file_name_end;
			}
			char* utf8_original_url_end = (char*)utf8_search_original_url_result + sizeof("\"original\":\"") - 1;
			for (; *utf8_original_url_end != 0; utf8_original_url_end++)
			{
				if (*utf8_original_url_end == '"')
					break;

				utf8_original_url += *utf8_original_url_end;
			}

			utf8_file_name += ".jpg";
			auto uni_file_name = utf82uni(utf8_file_name);
			for (auto& ch : uni_file_name)
			{
				if (ch == L'\\' ||
					ch == L'/' ||
					ch == L':' ||
					ch == L'*' ||
					ch == L'?' ||
					ch == L'>' ||
					ch == L'<' ||
					ch == L'|')
					ch = L'-';
			}

			FILE* pfile = nullptr;
			if (!_wfopen_s(&pfile, uni_file_name.c_str(), L"wb") && pfile)
			{
				curlpp::Easy request_picture;
				request_picture.setOpt(cURLpp::Options::Proxy("127.0.0.1"));
				request_picture.setOpt(cURLpp::Options::ProxyPort(10809));
				request_picture.setOpt(cURLpp::Options::HttpProxyTunnel(true));
				request_picture.setOpt(cURLpp::Options::Url(utf8_original_url));
				request_picture.setOpt(cURLpp::Options::WriteFile(pfile));
				request_picture.setOpt(cURLpp::Options::UserAgent("Mozilla/5.0"));
				request_picture.setOpt(cURLpp::Options::Referer("https://www.pixiv.net/ranking.php?mode=daily&content=illust"));

				request_picture.perform();

				fclose(pfile);

				return true;
			}
			else
			{
				BOOST_LOG_TRIVIAL(error) << "fopen_s error" << std::endl;
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(error) << "regex_search error" << std::endl;
		}
	}
	catch (curlpp::RuntimeError& e)
	{
		BOOST_LOG_TRIVIAL(error) << e.what() << std::endl;
	}
	catch (curlpp::LogicError& e)
	{
		BOOST_LOG_TRIVIAL(error) << e.what() << std::endl;
	}

	return false;
}

void work_thread(int64_t begin, int64_t end)
{
	bool is_exit = false;

	while (!is_exit)
	{
		try
		{
			for (int64_t i = begin; i < end; i++)
			{
				curlpp::Cleanup cleanup;

				std::string url = "https://www.pixiv.net/ranking.php?mode=daily&content=illust&p=";
				url += std::to_string(i);
				url += "&format=json";
				std::ostringstream utf8_response_html;
				curlpp::Easy request_html;
				request_html.setOpt(cURLpp::Options::Proxy("127.0.0.1"));
				request_html.setOpt(cURLpp::Options::ProxyPort(10809));
				request_html.setOpt(cURLpp::Options::HttpProxyTunnel(true));
				request_html.setOpt(cURLpp::Options::Url(url));
				request_html.setOpt(cURLpp::Options::WriteStream(&utf8_response_html));
				request_html.setOpt(cURLpp::Options::UserAgent("Mozilla/5.0"));

				BOOST_LOG_TRIVIAL(debug) << "search url:" << url << std::endl;

				request_html.perform();

				std::string utf8_response_html_string = utf8_response_html.str();
				const char* utf8_search_id_result = utf8_response_html_string.c_str();

				while (utf8_search_id_result)
				{
					utf8_search_id_result = (char*)utf8str(utf8_search_id_result, "\"illust_id\":") + sizeof("\"illust_id\":") - 1;

					std::string utf8_artwork_url = "https://www.pixiv.net/artworks/";
					std::wstring uni_saved_as;

					for (auto utf8_id_end = utf8_search_id_result; *utf8_id_end != 0; utf8_id_end++)
					{
						if (*utf8_id_end == ',')
							break;

						utf8_artwork_url += *utf8_id_end;
					}

					auto ret = getSinglePicture(utf8_artwork_url);

					g_locker.lock();
					if (ret)
					{
						BOOST_LOG_TRIVIAL(debug) << "current artwork url:" << utf8_artwork_url << std::endl;
						BOOST_LOG_TRIVIAL(info) << "downloading " << g_count << " picture" << std::endl;
					}
					else
						BOOST_LOG_TRIVIAL(error) << "downloading " << g_count << " picture error" << std::endl;
					g_locker.unlock();
				}

				is_exit = true;
			}
		}
		catch (curlpp::RuntimeError& e)
		{
			BOOST_LOG_TRIVIAL(error) << e.what() << std::endl;
		}
		catch (curlpp::LogicError& e)
		{
			BOOST_LOG_TRIVIAL(error) << e.what() << std::endl;
		}
	}
}

int main()
{
	SetConsoleOutputCP(65001);
	CONSOLE_FONT_INFOEX Info = { 0 };
	Info.cbSize = sizeof(Info);
	Info.dwFontSize.Y = 16;
	Info.FontWeight = FW_NORMAL;
	wcscpy_s(Info.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &Info);

	BOOST_LOG_TRIVIAL(info) << "strating " << std::thread::hardware_concurrency() << " thread(s)" << std::endl;

	uint64_t last_start = 1;
	uint64_t thread_count = 1;

	std::vector<std::thread*> thread_pool;
	for (size_t i = 0; i < std::thread::hardware_concurrency(); i++)
	{
		BOOST_LOG_TRIVIAL(info) << "strat " << thread_count << " thread,begin:" << last_start << ",end:" << last_start + 12 / std::thread::hardware_concurrency() << std::endl;

		thread_pool.push_back(new std::thread(work_thread, last_start, last_start + 12 / std::thread::hardware_concurrency()));

		last_start += 12 / std::thread::hardware_concurrency();

		++thread_count;
	}

	for (auto& thread_item : thread_pool)
		thread_item->join();

	for (auto& thread_item : thread_pool)
		delete thread_item;

	return 0;
}