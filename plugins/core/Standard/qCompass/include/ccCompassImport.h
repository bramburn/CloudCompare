#ifndef CCCOMPASSIMPORT_H
#define CCCOMPASSIMPORT_H

/**
 * @file ccCompassImport.h
 *
 * @brief Compass import functions
 *
 * Import foliation and lineation data.
 */

class QString;

class ccMainAppInterface;

/**
 * @brief Compass import functions
 */
namespace ccCompassImport
{
	/**
	 * @brief Import foliation data
	 * @param[in] app Main application interface
	 */
	void importFoliations( ccMainAppInterface *app );
	
	/**
	 * @brief Import lineation data
	 * @param[in] app Main application interface
	 */
	void importLineations( ccMainAppInterface *app );
};

#endif // CCCOMPASSIMPORT_H
