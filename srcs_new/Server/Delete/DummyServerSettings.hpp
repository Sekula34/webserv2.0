#ifndef DUMMYSERVERSETTINGS_HPP
# define DUMMYSERVERSETTINGS_HPP

# include <string>
# include <map>

class DummyServerSettings
{
	public:
		// Methods
		void			parseFile(const std::string& serverSettings);
		std::string		getValue(const std::string& key) const;
		void			printAllValues() const;
		DummyServerSettings();

	private:
		// Attributes
		std::map<std::string, std::string>	_mappedFile;

};

#endif
