// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccSingleton.h
 *
 * @brief Generic singleton template for managing single instances.
 *
 * @details Simple singleton wrapper template for managing single instances
 * of a class.
 *
 * ## Overview
 *
 * ccSingleton provides a simple pattern for singleton instances:
 * - Manages a single instance of type T
 * - Provides lazy initialization
 * - Automatic cleanup on destruction
 *
 * ## Thread Safety
 *
 * @warning This template is NOT thread-safe. External synchronization
 * is required for multi-threaded access.
 *
 * ## Usage
 *
 * @code
 * // Define singleton holder
 * struct ccMySingleton : public ccSingleton<ccMyClass> {
 *     ccMyClass* get() {
 *         if (!instance) {
 *             instance = new ccMyClass();
 *         }
 *         return instance;
 *     }
 * };
 *
 * // Use singleton
 * ccMyClass* obj = ccMySingleton::instance;
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#ifndef CC_SINGLETON_HEADER
#define CC_SINGLETON_HEADER

/**
 * @brief Singleton wrapper template.
 *
 * @tparam T Type to manage as singleton.
 */
template <class T>
struct ccSingleton
{
	/**
	 * @brief Default constructor.
	 */
	ccSingleton()
	    : instance(nullptr)
	{
	}

	/**
	 * @brief Destructor.
	 *
	 * Releases the managed instance.
	 */
	~ccSingleton()
	{
		release();
	}

	/**
	 * @brief Release the instance.
	 *
	 * Deletes the managed instance if it exists.
	 */
	inline void release()
	{
		if (instance)
		{
			delete instance;
			instance = nullptr;
		}
	}

	//! Current instance (nullptr if not created).
	T* instance;
};

#endif // CC_SINGLETON_HEADER
