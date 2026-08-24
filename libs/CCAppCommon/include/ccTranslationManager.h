#pragma once
// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: CloudCompare project                               #
// #                                                                        #
// ##########################################################################

/**
 * @file ccTranslationManager.h
 *
 * @brief Translation/internationalization manager
 *
 * Manages Qt translation (.ts) files and language switching.
 *
 * @author CloudCompare project
 */

#include "CCAppCommon.h"

// Qt
#include <QMenu>
#include <QPair>
#include <QVector>

/**
 * @brief Translation manager singleton
 *
 * Manages language translations for the application.
 */
class CCAPPCOMMON_LIB_API ccTranslationManager : public QObject
{
	Q_OBJECT

  public:
	/**
	 * @brief Get singleton instance
	 * @return Translation manager
	 */
	static ccTranslationManager& Get();

	/**
	 * @brief Destructor
	 */
	~ccTranslationManager() override = default;

	/**
	 * @brief Register translator file prefix
	 * @param[in] prefix File prefix (e.g. "CloudCompare")
	 * @param[in] path Path to translation files
	 */
	void registerTranslatorFile(const QString& prefix, const QString& path);

	/**
	 * @brief Load translations for current language
	 */
	inline void loadTranslations()
	{
		loadTranslation(languagePref());
	}

	/**
	 * @brief Load translations for specific language
	 * @param[in] language 2-letter ISO 639 language code
	 */
	void loadTranslation(QString language);

	/**
	 * @brief Populate menu with available languages
	 * @param[in] menu Menu to populate
	 * @param[in] pathToTranslationFiles Path to translation files
	 */
	void populateMenu(QMenu* menu, const QString& pathToTranslationFiles);

  protected:
	/**
	 * @brief Create translation manager
	 */
	explicit ccTranslationManager() = default;

  private: // methods
	/**
	 * @brief Translator file info
	 */
	struct CCAPPCOMMON_LIB_API TranslatorFile
	{
		QString prefix; //!< File prefix
		QString path;   //!< Path
	};
	using TranslatorFileList = QVector<TranslatorFile>;

	using TranslationInfo = QPair<QString, QString>;
	using LanguageList = QVector<TranslationInfo>;

	/// Get language preference
	QString languagePref() const;

	/// Get available languages
	LanguageList availableLanguages(const QString& appName, const QString& pathToTranslationFiles) const;

	/// Set language preference
	void setLanguagePref(const QString& languageCode);

  private: // members
	/// Registered translator files
	TranslatorFileList mTranslatorFileInfo;
};
