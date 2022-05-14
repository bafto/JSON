#include "json.h"
#include <iostream>
#include <string>
#include <string_view>
#include <regex>

namespace json
{
	Object::Object(std::initializer_list<std::pair<std::string, Value>> list)
	{
		for (auto it = list.begin(); it != list.end(); it++)
		{
			properties[it->first] = it->second;
		}
	}

	Value& Object::operator[](std::string str)
	{
		return properties[std::move(str)];
	}
	const Value& Object::operator[](std::string str) const
	{
		return properties.at(str);
	}

	bool Object::hasProperty(std::string str) const
	{
		return properties.find(std::move(str)) != properties.end();
	}

	static std::string repeatstr(const std::string& str, size_t n)
	{
		std::string ret;
		for (size_t i = 0; i < n; i++)
			ret += str;
		return ret;
	}

	static int indentLevel = 0; // used for Object::to_string and ArrayToString

	std::string Object::to_string(const std::string& indent) const
	{
		if (properties.empty())
			return "{}";

		std::string ret;

		if (indent.empty())
		{
			ret += "{";
			for (const auto& property : properties)
			{
				ret += '"' + property.first + "\":" + property.second.to_string("") + ',';
			}
			ret[ret.length()-1] = '}'; // set the trailing , to }
		}
		else
		{
			ret += "{\n";
			for (const auto& property : properties)
			{
				indentLevel++;
				ret += repeatstr(indent, indentLevel) + '"' + property.first + "\": " + property.second.to_string(indent) + ",\n";
				indentLevel--;
			}
			ret.erase(ret.end() - 2, ret.end()); // remove trailing ,\n
			ret += "\n" + repeatstr(indent, indentLevel) + "}";
		}

		return ret;
	}

	template<>
	std::string fromJsonValue<std::string>(const Value& val)
	{
		return std::get<std::string>(val.value);
	}
	template<>
	int fromJsonValue<int>(const Value& val)
	{
		return std::get<int>(val.value);
	}
	template<>
	float fromJsonValue<float>(const Value& val)
	{
		return std::get<float>(val.value);
	}
	template<>
	bool fromJsonValue<bool>(const Value& val)
	{
		return std::get<bool>(val.value);
	}
	template<>
	Object fromJsonValue<Object>(const Value& val)
	{
		return std::get<Object>(val.value);
	}
	template<>
	Array fromJsonValue<Array>(const Value& val)
	{
		return std::get<Array>(val.value);
	}
	template<>
	Null fromJsonValue<Null>(const Value& val)
	{
		return std::get<Null>(val.value);
	}

	template<json_value T>
	Value toJsonValue(const T& val)
	{
		return Value();
	}

	template<>
	Value toJsonValue<std::string>(const std::string& val)
	{
		return Value(val);
	}
	template<>
	Value toJsonValue<int>(const int& val)
	{
		return Value(val);
	}
	template<>
	Value toJsonValue<float>(const float& val)
	{
		return Value(val);
	}
	template<>
	Value toJsonValue<bool>(const bool& val)
	{
		return Value(val);
	}
	template<>
	Value toJsonValue<Object>(const Object& val)
	{
		return Value(val);
	}
	template<>
	Value toJsonValue<Array>(const Array& val)
	{
		return Value(val);
	}
	template<>
	Value toJsonValue<Null>(const Null& val)
	{
		return Value(val);
	}

	template<>
	std::string Value::as<std::string>() const
	{
		return fromJsonValue<std::string>(*this);
	}
	template<>
	int Value::as<int>() const
	{
		return fromJsonValue<int>(*this);
	}
	template<>
	float Value::as<float>() const
	{
		return fromJsonValue<float>(*this);
	}
	template<>
	bool Value::as<bool>() const
	{
		return fromJsonValue<bool>(*this);
	}
	template<>
	Object Value::as<Object>() const
	{
		return fromJsonValue<Object>(*this);
	}
	template<>
	Array Value::as<Array>() const
	{
		return fromJsonValue<Array>(*this);
	}
	template<>
	Null Value::as<Null>() const
	{
		return fromJsonValue<Null>(*this);
	}
	Value::operator std::string() const
	{
		return fromJsonValue<std::string>(*this);
	}
	Value::operator int() const
	{
		return fromJsonValue<int>(*this);
	}
	Value::operator float() const
	{
		return fromJsonValue<float>(*this);
	}
	Value::operator bool() const
	{
		return fromJsonValue<bool>(*this);
	}
	Value::operator Object() const
	{
		return fromJsonValue<Object>(*this);
	}
	Value::operator Array() const
	{
		return fromJsonValue<Array>(*this);
	}
	Value::operator Null() const
	{
		return fromJsonValue<Null>(*this);
	}

	static std::string ArrayToString(const Array& arr, const std::string& indent = "\t")
	{
		if (arr.empty())
			return "[]";

		std::string ret;

		if (indent.empty())
		{
			ret += "[";
			for (const auto& value : arr)
			{
				ret += value.to_string("") + ',';
			}
			ret[ret.length() - 1] = ']'; // set the trailing , to ]
		}
		else
		{
			ret += "[\n";
			for (const auto& value : arr)
			{
				indentLevel++; // indent level is defined above Object::to_string
				ret += repeatstr(indent, indentLevel) + value.to_string(indent) + ",\n";
				indentLevel--;
			}
			ret.erase(ret.end() - 2, ret.end()); // remove trailing ,\n
			ret += "\n" + repeatstr(indent, indentLevel) + "]";
		}

		return ret;
	}

	std::string Value::to_string(const std::string& indent) const
	{
		switch (value.index())
		{
		case 0:
			return "null";
		case 1:
			return (std::string)*this;
		case 2:
			return std::to_string((int)*this);
		case 3:
			return std::to_string((float)*this);
		case 4:
			return ((bool)*this) ? "true" : "false";
		case 5:
			return ((Object)*this).to_string(indent);
		case 6:
			return ArrayToString(((Array)*this), indent);
		}
		return ""; // unreachable
	}
	Value::Type Value::type() const
	{
		return (Type)value.index();
	}

	//helper for parseSingleValue

	static bool isDigit(char c)
	{
		return c >= '0' && c <= '9';
	}

	static bool isWhitespace(char c)
	{
		switch (c)
		{
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			return true;
		default:
			return false;
		}
	}

	static size_t skipWhitespace(std::string_view& str)
	{
		size_t i = 0;
		for (; i < str.length(); i++)
		{
			if (!isWhitespace(str[i]))
				break;
		}
		str.remove_prefix(i);
		return i;
	}

	static std::pair<Value, size_t> parseNumber(std::string_view str)
	{
		size_t pos;
		int i = 0;
		try
		{
			i = std::stoi(str.data(), &pos);
		}
		catch (const std::out_of_range& ex)
		{
			// TODO: handle later
			throw ex;
		}

		float f = 0.f;
		if (pos < str.size() && str[pos] == '.')
		{
			try
			{
				f = std::stof(str.data(), &pos);
				return { f, pos };
			}
			catch (const std::out_of_range& ex)
			{
				// TODO: handle later
				throw ex;
			}
		}
		return { i, pos };
	}

	static std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
		}
		return str;
	}

	static std::pair<std::string, size_t> parseString(std::string_view str)
	{
		str.remove_prefix(1); // remove leading "

		std::string temp(str);

		size_t end = temp.find("\"");
		if (end == std::string::npos) throw;
		for (;end != std::string::npos; end = temp.find("\"", end + 1))
			if (temp[end - 1] != '\\') break;

		temp = temp.substr(0, end);
		temp.shrink_to_fit();

		temp = ReplaceAll(std::move(temp), "\\\"", "\"");
		temp = ReplaceAll(std::move(temp), "\\\\", "\\");
		temp = ReplaceAll(std::move(temp), "\\/", "/");
		temp = ReplaceAll(std::move(temp), "\\b", "\b");
		temp = ReplaceAll(std::move(temp), "\\f", "\f");
		temp = ReplaceAll(std::move(temp), "\\n", "\n");
		temp = ReplaceAll(std::move(temp), "\\r", "\r");
		temp = ReplaceAll(std::move(temp), "\\t", "\t");

		return { temp, end + 2 }; // end + ""
	}

	// helper for parse
	// if the json is invalid returns an empty optional
	// otherwise returns the Value and the number of parsed characters
	static std::optional<std::pair<Value, size_t>> parseSingleValue(std::string_view str) 
	{
		size_t len = skipWhitespace(str);
		if (str.empty()) return {};

		if (isDigit(str.front()))
		{
			try
			{
				auto result = parseNumber(str);
				result.second += len;
				return result;
			}
			catch (...)
			{
				return {};
			}
		}

		if (str.starts_with("null")) return std::make_pair(null, len + 4);
		if (str.starts_with("true")) return std::make_pair(true, len + 4);
		if (str.starts_with("false")) return std::make_pair(false, len + 5);

		switch (str.front())
		{
		case '"':
			try
			{
				auto result = parseString(str);
				result.second += len;
				return result;
			}
			catch (...)
			{
				return {};
			}
		case '{':
		{
			try
			{
				str.remove_prefix(1); // remove leading {
				len += 1;
				Object object;

				len += skipWhitespace(str);
				if (str.empty() || str.front() == '}') return len += 1, std::make_pair(object, len);

				while(true)
				{
					len += skipWhitespace(str);
					if (str.empty() || str.front() != '"') return {}; // invalid string literal
					auto strLit = parseString(str);
					str.remove_prefix(strLit.second), len += strLit.second;
					len += skipWhitespace(str);
					if (str.empty() || str.front() != ':') return {};
					str.remove_prefix(1), len += 1; // remove :
					len += skipWhitespace(str);
					if (auto value = parseSingleValue(str))
					{
						len += value.value().second;
						str.remove_prefix(value.value().second);
						object[strLit.first] = std::move(value.value().first);
					}
					else return {};
					len += skipWhitespace(str);
					if (str.empty() || str.front() != ',') break;
					str.remove_prefix(1), len += 1; // remove ,
				}
				len += skipWhitespace(str);
				if (str.empty() || str.front() != '}') return {};
				len += 1; // }

				return std::make_pair(object, len);
			}
			catch (...)
			{
				return {};
			}
		}
		case '[':
			try
			{
				str.remove_prefix(1);
				len += 1;
				Array array;

				len += skipWhitespace(str);
				if (str.empty() || str.front() == ']') return len += 1, std::make_pair(array, len);

				while (true)
				{
					len += skipWhitespace(str);
					if (auto value = parseSingleValue(str))
					{
						len += value.value().second;
						str.remove_prefix(value.value().second);
						array.push_back(std::move(value.value().first));
					}
					else return {};
					len += skipWhitespace(str);
					if (str.empty() || str.front() != ',') break;
					str.remove_prefix(1), len += 1; // remove ,
				}
				len += skipWhitespace(str);
				if (str.empty() || str.front() != ']') return {};
				len += 1; // ]

				return std::make_pair(array, len);
			}
			catch (...)
			{
				return {};
			}
		}

		return {};
	}

	std::optional<Value> parse(std::string_view str)
	{
		if (auto opt = parseSingleValue(str))
		{
			if (skipWhitespace(str = str.substr(opt.value().second, str.length() - opt.value().second)), str.length() > 0)
				return {};
			return opt.value().first;
		}
		return {};
	}
}

std::ostream& operator<<(std::ostream& ostr, json::Value val)
{
	ostr << val.to_string();
	return ostr;
}
