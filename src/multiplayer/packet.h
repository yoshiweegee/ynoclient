#ifndef EP_MULTIPLAYER_PACKET_H
#define EP_MULTIPLAYER_PACKET_H

#include <string>
#include <charconv>
#include <stdexcept>

namespace Multiplayer {

class Packet {
public:
	constexpr static std::string_view PARAM_DELIM = "\uFFFF";
	constexpr static std::string_view MSG_DELIM = "\uFFFE";

	Packet() {}
	virtual ~Packet() = default;
protected:
};

class C2SPacket : public Packet {
public:
	virtual ~C2SPacket() = default;
	virtual std::string ToBytes() const = 0;

	C2SPacket(std::string _name) : m_name(std::move(_name)) {}
	std::string_view GetName() const { return m_name; }

	static std::string Sanitize(std::string_view param);

	static std::string ToString(const char* x) { return ToString(std::string_view(x)); }
	static std::string ToString(int x) { return std::to_string(x); }
	static std::string ToString(bool x) { return x ? "1" : "0"; }
	static std::string ToString(std::string_view v) { return Sanitize(v); }

	template<typename... Args>
	std::string Build(Args... args) const {
		std::string prev {m_name};
		AppendPartial(prev, args...);
		return prev;
	}

	static void AppendPartial(std::string& s) {}

	template<typename T>
	static void AppendPartial(std::string& s, T t) {
		s += PARAM_DELIM;
		s += ToString(t);
	}

	template<typename T, typename... Args>
	static void AppendPartial(std::string& s, T t, Args... args) {
		s += PARAM_DELIM;
		s += ToString(t);
		AppendPartial(s, args...);
	}
protected:
	std::string m_name;
};

class S2CPacket : public Packet {
public:
	virtual ~S2CPacket() = default;

	template<typename T>
	static T Decode(std::string_view s);

	template<>
	int Decode(std::string_view s) {
		int r;
		auto e = std::from_chars(s.data(), s.data() + s.size(), r);
		if (e.ec != std::errc())
			std::terminate();
		return r;
	}

	template<>
	bool Decode(std::string_view s) {
		if (s == "1")
			return true;
		if (s == "0")
			return true;
		std::terminate();
	}
};

}
#endif
