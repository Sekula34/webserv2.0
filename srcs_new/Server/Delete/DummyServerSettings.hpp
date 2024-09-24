#ifndef DUMMYSERVERSETTINGS_HPP
# define DUMMYSERVERSETTINGS_HPP

# include <string>
# include <map>

// For testing
# define CONFIGFILE "DummyConfig.txt"

class DummyServerSettings
{
	public:
		// Methods
		void			loadFile(const std::string& serverSettings);
		std::string		getValue(const std::string& key) const;
		// void			printAllValues() const;

	private:
		// Attributes
		std::map<std::string, std::string>	_mappedFile;

	public:
		DummyServerSettings(std::string type);
		DummyServerSettings();

	// Overloaded insertion operator (for testing).
	friend std::ostream&	operator<<(std::ostream& out, const DummyServerSettings& serverSettings);
};

#endif
