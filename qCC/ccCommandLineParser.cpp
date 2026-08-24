/**
 * @file ccCommandLineParser.cpp
 *
 * @brief Command-line parser implementation
 *
 * Implements the command-line interface for CloudCompare batch processing,
 * including command registration, argument parsing, entity management
 * (clouds/meshes), and export logic.
 *
 * @see ccCommandLineParser, ccCommandLineInterface, ccCommandLineCommands
 */

#include "ccCommandLineParser.h"

// Local
#include "ccCommandCrossSection.h"
#include "ccCommandLineCommands.h"
#include "ccCommandRaster.h"
#include "ccPluginInterface.h"

// qCC_db
#include <ccGenericMesh.h>
#include <ccHObjectCaster.h>
#include <ccProgressDialog.h>

// qCC_io
#include <AsciiFilter.h>
#include <BinFilter.h>

// qCC
#include "ccConsole.h"

#include <ui_commandLineDlg.h>

// Qt
#include <QDateTime>
#include <QElapsedTimer>
#include <QMessageBox>

// system
#include <unordered_set>

// Built-in command keywords
constexpr char COMMAND_HELP[] = "HELP";
constexpr char COMMAND_SILENT_MODE[] = "SILENT";

/*****************************************************/
/*************** ccCommandLineParser *****************/
/*****************************************************/

// ccCommandLineParser::printVerbose
void ccCommandLineParser::printVerbose(const QString& message) const
{
	ccConsole::PrintVerbose(message);
}

// ccCommandLineParser::print
void ccCommandLineParser::print(const QString& message) const
{
	ccConsole::Print(message);
}

// ccCommandLineParser::printHigh
void ccCommandLineParser::printHigh(const QString& message) const
{
	ccConsole::PrintHigh(message);
}

// ccCommandLineParser::printDebug
void ccCommandLineParser::printDebug(const QString& message) const
{
	ccConsole::PrintDebug(message);
}

// ccCommandLineParser::warning
void ccCommandLineParser::warning(const QString& message) const
{
	ccConsole::Warning(message);
}

// ccCommandLineParser::warningDebug
void ccCommandLineParser::warningDebug(const QString& message) const
{
	ccConsole::WarningDebug(message);
}

// ccCommandLineParser::error
bool ccCommandLineParser::error(const QString& message) const
{
	ccConsole::Error(message);
	return false;
}

// ccCommandLineParser::errorDebug
bool ccCommandLineParser::errorDebug(const QString& message) const
{
	ccConsole::ErrorDebug(message);
	return false;
}

// ccCommandLineParser::Parse
/**
 * @brief Main entry point for command-line processing
 *
 * Instantiates a ccCommandLineParser, registers built-in commands and
 * any plugin-supplied commands, then processes the argument list.
 *
 * Two modes of operation:
 * - Silent mode (-SILENT): no console widget, output to stdout
 * - Interactive mode: shows a console dialog and progress UI
 *
 * @param[in] arguments Full argument list (argv-style; index 0 = executable)
 * @param[in] plugins   List of available plugin interfaces for command registration
 * @return EXIT_SUCCESS (0) on complete success, EXIT_FAILURE on error
 */
int ccCommandLineParser::Parse(const QStringList& arguments, ccPluginInterfaceList& plugins)
{
	if (arguments.size() < 2)
	{
		assert(false);
		return EXIT_SUCCESS;
	}

	// Create the parser instance
	QScopedPointer<ccCommandLineParser> parser(new ccCommandLineParser);
	parser->registerBuiltInCommands();

	// Faster refresh for batch output
	ccConsole::SetRefreshCycle(200);

	// Handle single-quote argument grouping (e.g. -O 'my file with spaces.ply')
	{
		bool insideSingleQuoteSection = false;
		QString buffer;
		static const QChar SingleQuote{'\''};
		for (int currentArgIndex = 1; currentArgIndex < arguments.size(); ++currentArgIndex)
		{
			QString arg = arguments[currentArgIndex];
			// Argument starts with a single quote
			if (!insideSingleQuoteSection && arg.startsWith(SingleQuote))
			{
				if (arg.endsWith(SingleQuote))
				{
					// Complete quoted argument — nothing to strip
				}
				else
				{
					// Open quote — begin collecting continuation tokens
					insideSingleQuoteSection = true;
					buffer = arg.mid(1);
				}
			}
			else if (insideSingleQuoteSection)
			{
				buffer += QChar(' ') + arg;
				if (arg.endsWith(SingleQuote))
				{
					insideSingleQuoteSection = false;
					arg = buffer.left(buffer.length() - 1);
				}
			}

			if (!insideSingleQuoteSection)
			{
				parser->arguments().append(arg);
			}
		}

		if (insideSingleQuoteSection)
		{
			// Unclosed single quote — warn but try to proceed
			parser->warning("Probably malformed command (missing closing simple quote)");
			parser->arguments().append(buffer);
		}
	}

	// Detect silent mode (must be the first argument)
	if (ccCommandLineInterface::IsCommand(parser->arguments().front(), COMMAND_SILENT_MODE))
	{
		parser->arguments().pop_front();
		parser->toggleSilentMode(true);
	}

	// Set up the console widget or silent output
	QScopedPointer<QDialog> consoleDlg(nullptr);
	if (!parser->silentMode())
	{
		// Show the console dialog with embedded QListWidget
		consoleDlg.reset(new QDialog);
		Ui_commandLineDlg commandLineDlg;
		commandLineDlg.setupUi(consoleDlg.data());
		consoleDlg->show();
		ccConsole::Init(commandLineDlg.consoleWidget, consoleDlg.data());
		parser->fileLoadingParams().parentWidget = consoleDlg.data();
		QApplication::processEvents();
	}
	else
	{
		// Redirect all log output to stdout/stderr
		ccConsole::Init(nullptr, nullptr, nullptr, true);
	}

	// Register commands from plugins
	for (ccPluginInterface* plugin : plugins)
	{
		if (!plugin)
		{
			assert(false);
			continue;
		}
		plugin->registerCommands(parser.data());
	}

	// Run the argument processing loop
	int result = parser->start(consoleDlg.data());

	// Show completion message (non-silent mode only)
	if (!parser->silentMode())
	{
		if (result == EXIT_SUCCESS)
			QMessageBox::information(consoleDlg.data(), "Processed finished", "Job done");
		else
			QMessageBox::warning(consoleDlg.data(), "Processed finished", "An error occurred! Check console");
	}

	// Release resources in the right order
	parser->cleanup();
	parser.reset();
	ccConsole::ReleaseInstance();

	return result;
}

// ccCommandLineParser::ccCommandLineParser
ccCommandLineParser::ccCommandLineParser()
    : ccCommandLineInterface()
    , m_cloudExportFormat(BinFilter::GetFileFilter())
    , m_cloudExportExt(BinFilter::GetDefaultExtension())
    , m_meshExportFormat(BinFilter::GetFileFilter())
    , m_meshExportExt(BinFilter::GetDefaultExtension())
    , m_hierarchyExportFormat(BinFilter::GetFileFilter())
    , m_hierarchyExportExt(BinFilter::GetDefaultExtension())
    , m_orphans("orphans")
    , m_progressDialog(nullptr)
    , m_parentWidget(nullptr)
{
}

// ccCommandLineParser::~ccCommandLineParser
ccCommandLineParser::~ccCommandLineParser()
{
	if (m_progressDialog)
	{
		m_progressDialog->close();
		m_progressDialog->deleteLater();
	}
}

// ccCommandLineParser::registerCommand
/**
 * @brief Register a custom command with the parser
 *
 * Commands are stored in a QMap keyed by their keyword string (uppercase).
 * A keyword may only be registered once; attempting to register a duplicate
 * keyword returns false.
 *
 * @param[in] command Command object to register (must have a non-empty keyword)
 * @return true if registration succeeded; false if command is null or keyword
 *         is already taken
 */
bool ccCommandLineParser::registerCommand(Command::Shared command)
{
	if (!command)
	{
		assert(false);
		return false;
	}

	if (m_commands.contains(command->m_keyword))
	{
		assert(false);
		warning(QString("Internal error: keyword '%1' already registered (by command '%2')")
		            .arg(command->m_keyword, m_commands[command->m_keyword]->m_name));
		return false;
	}

	m_commands.insert(command->m_keyword, command);
	return true;
}

// ccCommandLineParser::getExportFilename
/**
 * @brief Generate an export filename for an entity
 *
 * Constructs the output filename as:
 * [path/][basename][_suffix][_timestamp][.extension]
 *
 * If the entity came from a multi-object file (indexInFile >= 0), the
 * index is prepended to the suffix.
 *
 * @param[in] entityDesc          Entity descriptor
 * @param[in] extension           File extension (auto-detected per entity type if empty)
 * @param[in] suffix              Optional user-defined suffix
 * @param[out] baseOutputFilename If non-null, receives the base output path without timestamp
 * @param[in] forceNoTimestamp   If true, suppress the timestamp component
 * @return The complete output filename (may be empty on error)
 */
QString ccCommandLineParser::getExportFilename(const CLEntityDesc& entityDesc,
                                               QString extension,
                                               QString suffix,
                                               QString* baseOutputFilename,
                                               bool forceNoTimestamp) const
{
	const ccHObject* entity = entityDesc.getEntity();
	if (!entity)
	{
		assert(false);
		warning("[getExportFilename] Internal error: invalid input entity!");
		return QString();
	}

	// Handle sub-items from multi-object files
	if (entityDesc.indexInFile >= 0)
	{
		if (suffix.isEmpty())
			suffix = QString("%1").arg(entityDesc.indexInFile);
		else
			suffix.prepend(QString("%1_").arg(entityDesc.indexInFile));
	}

	QString baseName = entityDesc.basename;
	if (!suffix.isEmpty())
	{
		baseName += QString("_") + suffix;
	}

	QString outputFilename = baseName;
	if (m_addTimestamp && !forceNoTimestamp)
	{
		outputFilename += QString("_%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh'h'mm_ss_zzz"));
	}

	if (!extension.isEmpty())
	{
		outputFilename += '.' + extension;
	}

	if (baseOutputFilename)
	{
		*baseOutputFilename = outputFilename;
	}

	if (!entityDesc.path.isEmpty())
	{
		outputFilename.prepend(entityDesc.path + '/');
	}

	return outputFilename;
}

// ccCommandLineParser::exportEntity
/**
 * @brief Export an entity to a file
 *
 * Determines the appropriate format (cloud/mesh/hierarchy) from the entity type
 * and any forced export options, then calls FileIOFilter::SaveToFile.
 *
 * Special handling for BIN format meshes: if the mesh's vertices cloud is not
 * a child of the mesh, it is temporarily attached before saving so the vertices
 * are bundled in the same file.
 *
 * @param[in,out] entityDesc          Entity to export (name may be updated with suffix)
 * @param[in]     suffix              Optional filename suffix
 * @param[out]    baseOutputFilename  If non-null, receives the base output path
 * @param[in]     options             Export option flags (ForceCloud, ForceMesh, etc.)
 * @return Empty string on success; error message string on failure
 */
QString ccCommandLineParser::exportEntity(CLEntityDesc& entityDesc,
                                          const QString& suffix,
                                          QString* baseOutputFilename,
                                          ccCommandLineInterface::ExportOptions options)
{
	print("[SAVING]");

	ccHObject* entity = entityDesc.getEntity();
	if (!entity)
	{
		assert(false);
		return "[ExportEntity] Internal error: invalid input entity!";
	}

	bool isCloud = entity->isA(CC_TYPES::POINT_CLOUD) || entityDesc.getCLEntityType() == CL_ENTITY_TYPE::CLOUD;
	bool isMesh = entity->isKindOf(CC_TYPES::MESH) || entityDesc.getCLEntityType() == CL_ENTITY_TYPE::MESH;

	QString extension = isCloud ? m_cloudExportExt : isMesh ? m_meshExportExt
	                                                        : m_hierarchyExportExt;
	QString format = isCloud ? m_cloudExportFormat : isMesh ? m_meshExportFormat
	                                                        : m_hierarchyExportFormat;

	// Allow forced export format overrides
	if (options.testFlag(ExportOption::ForceCloud))
	{
		extension = m_cloudExportExt;
		format = m_cloudExportFormat;
	}
	if (options.testFlag(ExportOption::ForceMesh))
	{
		extension = m_meshExportExt;
		format = m_meshExportFormat;
	}
	if (options.testFlag(ExportOption::ForceHierarchy))
	{
		extension = m_hierarchyExportExt;
		format = m_hierarchyExportFormat;
	}

	QString outputFilename = getExportFilename(entityDesc,
	                                           extension,
	                                           suffix,
	                                           baseOutputFilename,
	                                           options.testFlag(ExportOption::ForceNoTimestamp));
	if (outputFilename.isEmpty())
	{
		return QString();
	}

	// Update entity name with suffix
	{
		QString entName = entity->getName();
		if (entName.isEmpty())
		{
			entName = entityDesc.basename;
		}
		if (!suffix.isEmpty())
		{
			entName += QString("_") + suffix;
		}
		entity->setName(entName);
	}

	// For BIN meshes: temporarily attach the vertices cloud if not already a child
	bool tempDependencyCreated = false;
	ccGenericMesh* mesh = nullptr;
	if (entity->isKindOf(CC_TYPES::MESH) && m_meshExportFormat == BinFilter::GetFileFilter())
	{
		mesh = static_cast<ccGenericMesh*>(entity);
		ccGenericPointCloud* vertices = mesh->getAssociatedCloud();
		if (vertices && !mesh->isAncestorOf(vertices))
		{
			vertices->addChild(mesh, ccHObject::DP_NONE);
			entity = vertices;
			tempDependencyCreated = true;
		}
	}

	// Save with silent parameters (no dialog)
	FileIOFilter::SaveParameters parameters;
	{
		parameters.alwaysDisplaySaveDialog = false;
		if (!silentMode() && ccConsole::TheInstance())
		{
			parameters.parentWidget = ccConsole::TheInstance()->parentWidget();
		}
	}

#ifdef _DEBUG
	print("Output filename: " + outputFilename);
#endif
	CC_FILE_ERROR result = FileIOFilter::SaveToFile(entity, outputFilename, parameters, format);

	// Restore original state
	if (tempDependencyCreated)
	{
		if (mesh && entity)
		{
			entity->detachChild(mesh);
		}
		else
		{
			assert(false);
		}
	}

	return (result != CC_FERR_NO_ERROR
	            ? QString("Failed to save result in file '%1'").arg(outputFilename)
	            : QString());
}

/**********************************************************************/
/** SelectEntities<EntityDesc> — shared selection logic for clouds/meshes */
/**********************************************************************/

/**
 * @brief Shared selection logic for clouds or meshes
 *
 * Implements first/last/regex/all/ reverse selection on a vector of entity
 * descriptors. All entities start in unselected; criteria are applied to
 * move some to the selected vector.
 *
 * @tparam EntityDesc         CLCloudDesc or CLMeshDesc
 * @param[in] options         Selection criteria (first, last, regex, all, reverse)
 * @param[in] cmd             Command parser reference (for warnings/errors/printing)
 * @param[out] selectedEntities    Populated with selected entities
 * @param[out] unselectedEntities  Initially receives all entities; unselected remain
 * @param[in] entityType      Human-readable type name for warnings ("cloud"/"mesh")
 * @return true on success; false on std::bad_alloc or regex error
 */
template <class EntityDesc>
bool SelectEntities(ccCommandLineInterface::SelectEntitiesOptions options,
                    const ccCommandLineParser& cmd,
                    std::vector<EntityDesc>& selectedEntities,
                    std::vector<EntityDesc>& unselectedEntities,
                    QString entityType)
{
	// Warn if nothing loaded — don't abort the whole batch
	if (selectedEntities.empty() && unselectedEntities.empty())
	{
		cmd.warning(QObject::tr("\tNo %1 loaded. Load some with the -O command").arg(entityType));
		return true;
	}

	if (options.selectRegex && !options.regex.isValid())
	{
		return cmd.error(QObject::tr("Regex string invalid: %1").arg(options.regex.errorString()));
	}

	try
	{
		// Move all into unselected; we'll filter from there
		unselectedEntities.insert(unselectedEntities.end(),
		                          selectedEntities.begin(),
		                          selectedEntities.end());
		selectedEntities.clear();

		// Restore load order by unique ID
		std::sort(unselectedEntities.begin(), unselectedEntities.end(), [](const EntityDesc& a, const EntityDesc& b)
		          { return (a.getEntity()->getUniqueID() < b.getEntity()->getUniqueID()); });

		size_t lastIndex = unselectedEntities.size() - 1;
		size_t index = 0;

		for (auto it = unselectedEntities.begin(); it != unselectedEntities.end();)
		{
			QString nameToValidate = QObject::tr("%1/%2").arg(it->basename).arg(it->getEntity()->getName());
			bool toBeSelected = false;

			if (!options.reverse)
			{
				// Select first N
				if (options.selectFirst && index < static_cast<size_t>(options.firstNr))
					toBeSelected = true;
				// Select last N
				if (options.selectLast && index > lastIndex - static_cast<size_t>(options.lastNr))
					toBeSelected = true;
			}
			else
			{
				// Select NOT first N
				if (options.selectFirst && index >= static_cast<size_t>(options.firstNr) && !options.selectLast)
					toBeSelected = true;
				// Select NOT last N
				if (options.selectLast && index <= lastIndex - static_cast<size_t>(options.lastNr) && !options.selectFirst)
					toBeSelected = true;
				// Select NOT first N and NOT last N (middle)
				if (options.selectFirst && options.selectLast
				    && index >= static_cast<size_t>(options.firstNr)
				    && index <= lastIndex - static_cast<size_t>(options.lastNr))
					toBeSelected = true;
			}

			// Regex override
			if (options.selectRegex)
			{
				toBeSelected = options.regex.match(nameToValidate).hasMatch()
				                   ? !options.reverse
				                   : options.reverse;
			}

			// selectAll takes highest priority
			if (options.selectAll)
			{
				toBeSelected = !options.reverse;
			}

			if (toBeSelected)
			{
				cmd.print(QObject::tr("\t[*] UID: %2 name: %1").arg(nameToValidate).arg(it->getEntity()->getUniqueID()));
				selectedEntities.push_back(*it);
				it = unselectedEntities.erase(it);
			}
			else
			{
				cmd.print(QObject::tr("\t[ ] UID: %2 name: %1").arg(nameToValidate).arg(it->getEntity()->getUniqueID()));
				++it;
			}
			++index;
		}
	}
	catch (const std::bad_alloc&)
	{
		return cmd.error(QObject::tr("Not enough memory"));
	}

	return true;
}

// ccCommandLineParser::selectClouds
/**
 * @brief Apply selection criteria to loaded clouds
 * @param[in] options Selection criteria
 * @return true on success
 */
bool ccCommandLineParser::selectClouds(const SelectEntitiesOptions& options)
{
	return SelectEntities(options, *this, m_clouds, m_unselectedClouds, "cloud");
}

// ccCommandLineParser::selectMeshes
/**
 * @brief Apply selection criteria to loaded meshes
 * @param[in] options Selection criteria
 * @return true on success
 */
bool ccCommandLineParser::selectMeshes(const SelectEntitiesOptions& options)
{
	return SelectEntities(options, *this, m_meshes, m_unselectedMeshes, "mesh");
}

// ccCommandLineParser::removeClouds
/**
 * @brief Delete and remove loaded clouds
 *
 * All clouds in m_clouds are deleted (not just removed from the list) because
 * the parser owns the loaded entities.
 *
 * @param[in] onlyLast If true, delete only the most recently added cloud
 */
void ccCommandLineParser::removeClouds(bool onlyLast)
{
	while (!m_clouds.empty())
	{
		delete m_clouds.back().pc;
		m_clouds.pop_back();
		if (onlyLast)
			break;
	}
}

// ccCommandLineParser::removeMeshes
/**
 * @brief Delete and remove loaded meshes
 *
 * All meshes in m_meshes are deleted (not just removed from the list) because
 * the parser owns the loaded entities.
 *
 * @param[in] onlyLast If true, delete only the most recently added mesh
 */
void ccCommandLineParser::removeMeshes(bool onlyLast)
{
	while (!m_meshes.empty())
	{
		delete m_meshes.back().mesh;
		m_meshes.pop_back();
		if (onlyLast)
			break;
	}
}

// Static state for FIRST_GLOBAL_SHIFT mode
static bool s_firstCoordinatesShiftEnabled = false;
static CCVector3d s_firstGlobalShift;
static bool s_globalShiftFirstTime = true;

// ccCommandLineParser::setGlobalShiftOptions
/**
 * @brief Configure coordinate shift handling for subsequent file loads
 *
 * Sets m_loadingParameters based on the requested mode:
 * - NO_GLOBAL_SHIFT: no automatic shift
 * - AUTO_GLOBAL_SHIFT: let CC choose automatically
 * - FIRST_GLOBAL_SHIFT: reuse the shift from the first loaded file
 * - CUSTOM_GLOBAL_SHIFT: use a user-supplied shift vector
 *
 * @param[in] globalShiftOptions Mode and optional custom shift vector
 */
void ccCommandLineParser::setGlobalShiftOptions(const GlobalShiftOptions& globalShiftOptions)
{
	m_loadingParameters.shiftHandlingMode = ccGlobalShiftManager::NO_DIALOG;
	m_loadingParameters.coordinatesShiftEnabled = false;
	m_loadingParameters.coordinatesShift = CCVector3d(0, 0, 0);

	switch (globalShiftOptions.mode)
	{
	case GlobalShiftOptions::AUTO_GLOBAL_SHIFT:
		m_loadingParameters.shiftHandlingMode = ccGlobalShiftManager::NO_DIALOG_AUTO_SHIFT;
		break;

	case GlobalShiftOptions::FIRST_GLOBAL_SHIFT:
		if (s_globalShiftFirstTime)
		{
			ccLog::Warning("Can't reuse the first Global Shift (no global shift set yet)");
			m_loadingParameters.shiftHandlingMode = ccGlobalShiftManager::NO_DIALOG_AUTO_SHIFT;
		}
		else
		{
			m_loadingParameters.coordinatesShiftEnabled = s_firstCoordinatesShiftEnabled;
			m_loadingParameters.coordinatesShift = s_firstGlobalShift;
		}
		break;

	case GlobalShiftOptions::CUSTOM_GLOBAL_SHIFT:
		m_loadingParameters.coordinatesShiftEnabled = true;
		m_loadingParameters.coordinatesShift = globalShiftOptions.customGlobalShift;
		break;

	default:
		break;
	}
}

// ccCommandLineParser::updateInteralGlobalShift
/**
 * @brief Remember the first-loaded file's shift for reuse by FIRST_GLOBAL_SHIFT mode
 *
 * Called after each successful import. On the first call with a non-zero shift,
 * records the shift vector so subsequent files can reuse it.
 *
 * @param[in] globalShiftOptions Mode and shift vector from the last import
 */
void ccCommandLineParser::updateInteralGlobalShift(const GlobalShiftOptions& globalShiftOptions)
{
	if (globalShiftOptions.mode != GlobalShiftOptions::NO_GLOBAL_SHIFT)
	{
		if (s_globalShiftFirstTime)
		{
			s_firstCoordinatesShiftEnabled = m_loadingParameters.coordinatesShiftEnabled;
			s_firstGlobalShift = m_loadingParameters.coordinatesShift;
			s_globalShiftFirstTime = false;
		}
	}
}

// ccCommandLineParser::importFile
/**
 * @brief Load a file and register its clouds and meshes
 *
 * Calls FileIOFilter::LoadFromFile, then scans the returned hierarchy for:
 * 1. Meshes (separating real meshes from sub-meshes)
 * 2. Point clouds that are not mesh vertices
 *
 * Detached clouds/meshes are added to m_meshes/m_clouds. Vertex clouds
 * belonging to meshes are added to the orphans container.
 *
 * @param[in] filename             Path to the file to load
 * @param[in] globalShiftOptions    Coordinate shift handling mode
 * @param[in] filter                Optional pre-selected file filter
 * @return true if the file was loaded and at least one entity registered
 */
bool ccCommandLineParser::importFile(QString filename, const GlobalShiftOptions& globalShiftOptions, FileIOFilter::Shared filter)
{
	printHigh(QString("Opening file: '%1'").arg(filename));

	setGlobalShiftOptions(globalShiftOptions);

	CC_FILE_ERROR result = CC_FERR_NO_ERROR;
	ccHObject* db = nullptr;
	if (filter)
	{
		db = FileIOFilter::LoadFromFile(filename, m_loadingParameters, filter, result);
	}
	else
	{
		db = FileIOFilter::LoadFromFile(filename, m_loadingParameters, result, QString());
	}

	if (!db)
	{
		return false;
	}

	updateInteralGlobalShift(globalShiftOptions);

	// Track which cloud IDs belong to meshes (so we don't treat them as standalone clouds)
	std::unordered_set<unsigned> verticesIDs;

	// Pass 1: find real meshes (top-level only)
	{
		ccHObject::Container meshes;
		size_t count = 0;
		if (db->filterChildren(meshes, true, CC_TYPES::MESH, true) != 0)
		{
			count += meshes.size();
			for (size_t i = 0; i < meshes.size(); ++i)
			{
				ccGenericMesh* mesh = ccHObjectCaster::ToGenericMesh(meshes[i]);
				if (mesh->getParent())
					mesh->getParent()->detachChild(mesh);

				ccGenericPointCloud* vertices = mesh->getAssociatedCloud();
				if (vertices)
				{
					verticesIDs.insert(vertices->getUniqueID());
					print(QString("Found one mesh with %1 faces and %2 vertices: '%3'")
					          .arg(mesh->size())
					          .arg(mesh->getAssociatedCloud()->size())
					          .arg(mesh->getName()));
					m_meshes.emplace_back(mesh, filename, count == 1 ? -1 : static_cast<int>(i));
				}
				else
				{
					delete mesh;
					assert(false);
				}
			}
		}

		// Pass 2: find sub-meshes
		meshes.clear();
		if (db->filterChildren(meshes, true, CC_TYPES::MESH, false) != 0)
		{
			size_t countBefore = count;
			count += meshes.size();
			for (size_t i = 0; i < meshes.size(); ++i)
			{
				ccGenericMesh* mesh = ccHObjectCaster::ToGenericMesh(meshes[i]);
				if (mesh->getParent())
					mesh->getParent()->detachChild(mesh);

				ccGenericPointCloud* vertices = mesh->getAssociatedCloud();
				if (vertices)
				{
					verticesIDs.insert(vertices->getUniqueID());
					print(QString("Found one kind of mesh with %1 faces and %2 vertices: '%3'")
					          .arg(mesh->size())
					          .arg(mesh->getAssociatedCloud()->size())
					          .arg(mesh->getName()));
					m_meshes.emplace_back(mesh, filename, count == 1 ? -1 : static_cast<int>(countBefore + i));
				}
				else
				{
					delete mesh;
					assert(false);
				}
			}
		}
	}

	// Pass 3: find standalone point clouds (excluding mesh vertices)
	{
		ccHObject::Container clouds;
		db->filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);
		size_t count = clouds.size();
		for (size_t i = 0; i < count; ++i)
		{
			ccPointCloud* pc = static_cast<ccPointCloud*>(clouds[i]);
			if (pc->getParent())
				pc->getParent()->detachChild(pc);

			// Skip clouds that are mesh vertices
			if (verticesIDs.find(pc->getUniqueID()) != verticesIDs.end())
			{
				m_orphans.addChild(pc);
				continue;
			}

			print(QString("Found one cloud with %1 points").arg(pc->size()));
			m_clouds.emplace_back(pc, filename, count == 1 ? -1 : static_cast<int>(i));
		}
	}

	delete db;
	return true;
}

// ccCommandLineParser::saveClouds
/**
 * @brief Save all loaded clouds to files
 *
 * Two modes:
 * - allAtOnce: combines all clouds into a single file (if the format supports it)
 * - standard: one file per cloud
 *
 * @param[in] suffix              Optional filename suffix
 * @param[in] allAtOnce           Combine all clouds into one file
 * @param[in] allAtOnceFileName   Output filename for all-at-once mode
 * @return true on success
 */
bool ccCommandLineParser::saveClouds(QString suffix, bool allAtOnce, const QString* allAtOnceFileName)
{
	if (allAtOnce)
	{
		FileIOFilter::Shared filter = FileIOFilter::GetFilter(m_cloudExportFormat, false);
		bool multiple = false;
		if (filter)
		{
			bool exclusive = true;
			filter->canSave(CC_TYPES::POINT_CLOUD, multiple, exclusive);
		}

		if (multiple)
		{
			ccHObject tempContainer("Clouds");
			{
				for (CLCloudDesc& desc : m_clouds)
				{
					tempContainer.addChild(desc.getEntity(), ccHObject::DP_NONE);
				}
			}

			CLGroupDesc desc(&tempContainer, "AllClouds", m_clouds.front().path);
			if (allAtOnceFileName)
				CommandSave::SetFileDesc(desc, *allAtOnceFileName);

			QString errorStr = exportEntity(desc, suffix, nullptr, ExportOption::ForceCloud);
			if (!errorStr.isEmpty())
				return error(errorStr);
			return true;
		}
		else
		{
			error(QString("The currently selected output format for clouds (%1) doesn't handle multiple entities at once!")
			          .arg(m_cloudExportFormat));
		}
	}

	// Standard: one file per cloud
	for (CLCloudDesc& desc : m_clouds)
	{
		QString errorStr = exportEntity(desc, suffix);
		if (!errorStr.isEmpty())
			return error(errorStr);
	}

	return true;
}

// ccCommandLineParser::saveMeshes
/**
 * @brief Save all loaded meshes to files
 *
 * Two modes:
 * - allAtOnce: combines all meshes into a single file (if the format supports it)
 * - standard: one file per mesh
 *
 * @param[in] suffix              Optional filename suffix
 * @param[in] allAtOnce           Combine all meshes into one file
 * @param[in] allAtOnceFileName   Output filename for all-at-once mode
 * @return true on success
 */
bool ccCommandLineParser::saveMeshes(QString suffix, bool allAtOnce, const QString* allAtOnceFileName)
{
	if (allAtOnce)
	{
		FileIOFilter::Shared filter = FileIOFilter::GetFilter(m_meshExportFormat, false);
		bool multiple = false;
		if (filter)
		{
			bool exclusive = true;
			filter->canSave(CC_TYPES::MESH, multiple, exclusive);
		}

		if (multiple)
		{
			ccHObject tempContainer("Meshes");
			{
				for (auto& mesh : m_meshes)
				{
					tempContainer.addChild(mesh.getEntity(), ccHObject::DP_NONE);
				}
			}

			CLGroupDesc desc(&tempContainer, "AllMeshes", m_meshes.front().path);
			if (allAtOnceFileName)
				CommandSave::SetFileDesc(desc, *allAtOnceFileName);

			QString errorStr = exportEntity(desc, suffix, nullptr, ExportOption::ForceMesh);
			if (!errorStr.isEmpty())
				return error(errorStr);
			return true;
		}
		else
		{
			error(QString("The currently selected output format for meshes (%1) doesn't handle multiple entities at once!")
			          .arg(m_meshExportFormat));
		}
	}

	// Standard: one file per mesh
	for (auto& mesh : m_meshes)
	{
		QString errorStr = exportEntity(mesh, suffix);
		if (!errorStr.isEmpty())
			return error(errorStr);
	}

	return true;
}

// ccCommandLineParser::registerBuiltInCommands
/**
 * @brief Register all built-in commands with the parser
 *
 * Registers ~100 command classes including:
 * - File I/O: Load, Save, AutoSave, LogFile
 * - Transformation: ApplyTransformation, MatchBBCenters, MatchScales, etc.
 * - Filters: SORFilter, NoiseFilter, Filter
 * - Geometry: DelaunayTri, Volume25D, Rasterize, etc.
 * - Scalar fields: SFArithmetic, SFOperation, SFInterpolation, etc.
 * - Registration: ICP
 */
void ccCommandLineParser::registerBuiltInCommands()
{
	registerCommand(Command::Shared(new CommandDebugCmdLine));
	registerCommand(Command::Shared(new CommandLoad));
	registerCommand(Command::Shared(new CommandLoadCommandFile));
	registerCommand(Command::Shared(new CommandSubsample));
	registerCommand(Command::Shared(new CommandExtractCCs));
	registerCommand(Command::Shared(new CommandCurvature));
	registerCommand(Command::Shared(new CommandApproxDensity));
	registerCommand(Command::Shared(new CommandDensity));
	registerCommand(Command::Shared(new CommandSFGradient));
	registerCommand(Command::Shared(new CommandRoughness));
	registerCommand(Command::Shared(new CommandApplyTransformation));
	registerCommand(Command::Shared(new CommandDropGlobalShift));
	registerCommand(Command::Shared(new CommandFilterBySFValue));
	registerCommand(Command::Shared(new CommandMergeClouds));
	registerCommand(Command::Shared(new CommandMergeMeshes));
	registerCommand(Command::Shared(new CommandSetActiveSF));
	registerCommand(Command::Shared(new CommandSetGlobalShift));
	registerCommand(Command::Shared(new CommandRemoveAllSFs));
	registerCommand(Command::Shared(new CommandRemoveSF));
	registerCommand(Command::Shared(new CommandRemoveRGB));
	registerCommand(Command::Shared(new CommandRemoveNormals));
	registerCommand(Command::Shared(new CommandRemoveScanGrids));
	registerCommand(Command::Shared(new CommandRemoveSensors));
	registerCommand(Command::Shared(new CommandMatchBBCenters));
	registerCommand(Command::Shared(new CommandMatchScales));
	registerCommand(Command::Shared(new CommandMatchBestFitPlane));
	registerCommand(Command::Shared(new CommandOrientNormalsMST));
	registerCommand(Command::Shared(new CommandSORFilter));
	registerCommand(Command::Shared(new CommandNoiseFilter));
	registerCommand(Command::Shared(new CommandRemoveDuplicatePoints));
	registerCommand(Command::Shared(new CommandSampleMesh));
	registerCommand(Command::Shared(new CommandCompressFWF));
	registerCommand(Command::Shared(new CommandExtractVertices));
	registerCommand(Command::Shared(new CommandCrossSection));
	registerCommand(Command::Shared(new CommandCrop));
	registerCommand(Command::Shared(new CommandCrop2D));
	registerCommand(Command::Shared(new CommandCoordToSF));
	registerCommand(Command::Shared(new CommandSFToCoord));
	registerCommand(Command::Shared(new CommandNormToSF));
	registerCommand(Command::Shared(new CommandSFToNorm));
	registerCommand(Command::Shared(new CommandColorBanding));
	registerCommand(Command::Shared(new CommandColorLevels));
	registerCommand(Command::Shared(new CommandC2MDist));
	registerCommand(Command::Shared(new CommandC2CDist));
	registerCommand(Command::Shared(new CommandCPS));
	registerCommand(Command::Shared(new CommandStatTest));
	registerCommand(Command::Shared(new CommandStatFit));
	registerCommand(Command::Shared(new CommandDelaunayTri));
	registerCommand(Command::Shared(new CommandSFArithmetic));
	registerCommand(Command::Shared(new CommandSFOperation));
	registerCommand(Command::Shared(new CommandSFOperationSF));
	registerCommand(Command::Shared(new CommandSFInterpolation));
	registerCommand(Command::Shared(new CommandColorInterpolation));
	registerCommand(Command::Shared(new CommandFilter));
	registerCommand(Command::Shared(new CommandRenameEntities));
	registerCommand(Command::Shared(new CommandSFRename));
	registerCommand(Command::Shared(new CommandSFAddConst));
	registerCommand(Command::Shared(new CommandSFAddId));
	registerCommand(Command::Shared(new CommandICP));
	registerCommand(Command::Shared(new CommandChangeCloudOutputFormat));
	registerCommand(Command::Shared(new CommandChangeMeshOutputFormat));
	registerCommand(Command::Shared(new CommandChangeHierarchyOutputFormat));
	registerCommand(Command::Shared(new CommandChangePLYExportFormat));
	registerCommand(Command::Shared(new CommandPLYNoSFPrefix));
	registerCommand(Command::Shared(new CommandForceNormalsComputation));
	registerCommand(Command::Shared(new CommandSaveClouds));
	registerCommand(Command::Shared(new CommandSaveMeshes));
	registerCommand(Command::Shared(new CommandAutoSave));
	registerCommand(Command::Shared(new CommandLogFile));
	registerCommand(Command::Shared(new CommandSelectEntities));
	registerCommand(Command::Shared(new CommandClear));
	registerCommand(Command::Shared(new CommandClearClouds));
	registerCommand(Command::Shared(new CommandPopClouds));
	registerCommand(Command::Shared(new CommandClearMeshes));
	registerCommand(Command::Shared(new CommandPopMeshes));
	registerCommand(Command::Shared(new CommandSetNoTimestamp));
	registerCommand(Command::Shared(new CommandVolume25D));
	registerCommand(Command::Shared(new CommandRasterize));
	registerCommand(Command::Shared(new CommandOctreeNormal));
	registerCommand(Command::Shared(new CommandConvertNormalsToDipAndDipDir));
	registerCommand(Command::Shared(new CommandConvertNormalsToSFs));
	registerCommand(Command::Shared(new CommandConvertNormalsToHSV));
	registerCommand(Command::Shared(new CommandClearNormals));
	registerCommand(Command::Shared(new CommandInvertNormal));
	registerCommand(Command::Shared(new CommandComputeMeshVolume));
	registerCommand(Command::Shared(new CommandSFColorScale));
	registerCommand(Command::Shared(new CommandSFConvertToRGB));
	registerCommand(Command::Shared(new CommandMoment));
	registerCommand(Command::Shared(new CommandFeature));
	registerCommand(Command::Shared(new CommandRGBConvertToSF));
	registerCommand(Command::Shared(new CommandFlipTriangles));
	registerCommand(Command::Shared(new CommandSetVerbosity));
	registerCommand(Command::Shared(new CommandComputeDistancesFromSensor));
	registerCommand(Command::Shared(new CommandComputeScatteringAngles));
}

// ccCommandLineParser::cleanup
/**
 * @brief Release all loaded entities and clean up state
 *
 * Called at the end of batch processing to delete all clouds and meshes
 * owned by the parser.
 */
void ccCommandLineParser::cleanup()
{
	removeClouds();
	removeMeshes();
}

// ccCommandLineParser::start
/**
 * @brief Main argument-processing loop
 *
 * Repeatedly dequeues the next argument, looks up its keyword in the
 * registered command map, and calls Command::process(). Supports the
 * -HELP command which lists all available keywords.
 *
 * Each command's elapsed time is printed after it completes.
 *
 * @param[in] parent Parent dialog for progress indicators
 * @return EXIT_SUCCESS if all commands succeeded; EXIT_FAILURE otherwise
 */
int ccCommandLineParser::start(QDialog* parent)
{
	if (m_arguments.empty())
	{
		assert(false);
		return EXIT_FAILURE;
	}

	m_parentWidget = parent;

	QElapsedTimer eTimer;
	eTimer.start();

	bool success = true;
	while (success && !m_arguments.empty())
	{
		QApplication::processEvents();
		QString argument = m_arguments.takeFirst();

		if (!argument.startsWith("-"))
		{
			error(QString("Command expected (commands start with '-'). Found '%1'").arg(argument));
			success = false;
			break;
		}
		QString keyword = argument.mid(1).toUpper();

		if (m_commands.contains(keyword))
		{
			QElapsedTimer eTimerSubProcess;
			eTimerSubProcess.start();
			QString processName = m_commands[keyword]->m_name.toUpper();
			printHigh(QString("[%1] Command detected").arg(processName));
			success = m_commands[keyword]->process(*this);
			printHigh(QString("[%2] finished in %1 s.")
			              .arg(eTimerSubProcess.elapsed() / 1.0e3, 0, 'f', 2)
			              .arg(processName));
		}
		else if (keyword == COMMAND_SILENT_MODE)
		{
			warning(QString("Misplaced command: '%1' (must be first)").arg(COMMAND_SILENT_MODE));
		}
		else if (keyword == COMMAND_HELP)
		{
			print("Available commands:");
			for (auto it = m_commands.constBegin(); it != m_commands.constEnd(); ++it)
			{
				print(QString("-%1: %2").arg(it.key().toUpper(), it.value()->m_name));
			}
		}
		else
		{
			error(QString("Unknown or misplaced command: '%1'").arg(argument));
			success = false;
			break;
		}
	}

	print(QString("Processed finished in %1 s.").arg(eTimer.elapsed() / 1.0e3, 0, 'f', 2));

	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
