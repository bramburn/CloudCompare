// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        //
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        //
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccArray.h
 *
 * @brief Shareable array template for DB-integrated arrays.
 *
 * @details Template for arrays that can be shared between entities
 * and integrated into the database tree hierarchy.
 *
 * ## Overview
 *
 * ccArray combines:
 * - **std::vector**: Dynamic array storage
 * - **CCShareable**: Reference counting
 * - **ccHObject**: DB tree integration
 *
 * This allows arrays to be:
 * - Shared between multiple entities
 * - Inserted in the DB tree
 * - Serialized to files
 * - Cloned with reference counting
 *
 * ## Template Parameters
 *
 * - **Type**: Element type (e.g., CCVector3)
 * - **N**: Number of components per element
 * - **ComponentType**: Component data type
 *
 * ## Common Uses
 *
 * - **CCVector3** (N=3, float/double): Vertex positions
 * - Normals, colors, texture coordinates
 *
 * ## Usage
 *
 * @code
 * // Create array
 * ccArray<CCVector3, 3, float>* vertices = new ccArray<CCVector3, 3, float>("Vertices");
 *
 * // Add elements
 * vertices->addElement(CCVector3(0, 0, 0));
 * vertices->addElement(CCVector3(1, 0, 0));
 *
 * // Clone (shares data)
 * auto vertices2 = vertices->clone();
 *
 * // Reserve memory safely
 * if (!vertices->reserveSafe(1000)) {
 *     // Handle memory error
 * }
 *
 * // Insert in DB tree
 * dbRoot->addChild(vertices);
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see CCShareable for reference counting
 * @see ccHObject for tree integration
 */

#ifndef CC_ARRAY_HEADER
#define CC_ARRAY_HEADER

// Local
#include "ccHObject.h"

// CCCoreLib
#include <CCShareable.h>

// System
#include <vector>

/**
 * @brief Shareable array template.
 *
 * @details Combines std::vector storage with reference counting
 * and DB tree integration.
 *
 * Features:
 * - Dynamic array with safe memory management
 * - Reference counting via CCShareable
 * - DB tree hierarchy integration
 * - Serialization support
 * - Safe memory operations (no exceptions)
 *
 * @tparam Type Element type
 * @tparam N Number of components per element
 * @tparam ComponentType Component data type
 *
 * @extends std::vector<Type>
 * @extends CCShareable
 * @extends ccHObject
 */
template <class Type, int N, class ComponentType>
class ccArray : public std::vector<Type>
    , public CCShareable
    , public ccHObject
{
  public:
	//! Base type alias.
	typedef ccArray<Type, N, ComponentType> Base;

	/**
	 * @brief Create an array.
	 *
	 * @param[in] name Array name.
	 */
	ccArray(QString name = QString())
	    : ccHObject(name)
	{
		setFlagState(CC_LOCKED, true);
	}

	/**
	 * @brief Clone the array.
	 *
	 * @return New array with same content.
	 *
	 * @note Shares underlying data via reference counting.
	 */
	virtual Base* clone()
	{
		Base* cloneArray = new Base(getName());
		if (!copy(*cloneArray))
		{
			cloneArray->release();
			cloneArray = nullptr;
		}
		return cloneArray;
	}

	/**
	 * @brief Copy content to another array.
	 *
	 * @param[out] dest Destination array.
	 *
	 * @return true on success.
	 */
	bool copy(Base& dest) const
	{
		try
		{
			static_cast<std::vector<Type>&>(dest) = static_cast<const std::vector<Type>&>(*this);
		}
		catch (const std::bad_alloc&)
		{
			ccLog::Warning("[ccArray::copy] Not enough memory");
			return false;
		}
		return true;
	}

	/**
	 * @brief Reserve memory safely.
	 *
	 * @param[in] count Number of elements.
	 *
	 * @return true on success.
	 *
	 * @note No exceptions thrown.
	 */
	bool reserveSafe(size_t count)
	{
		try
		{
			this->reserve(count);
		}
		catch (const std::bad_alloc&)
		{
			return false;
		}
		return true;
	}

	/**
	 * @brief Check if memory is allocated.
	 */
	inline bool isAllocated() const
	{
		return this->capacity() != 0;
	}

	/**
	 * @brief Resize safely.
	 *
	 * @param[in] count New size.
	 * @param[in] initNewElements Initialize new elements.
	 * @param[in] valueForNewElements Value for new elements.
	 *
	 * @return true on success.
	 *
	 * @note No exceptions thrown.
	 */
	bool resizeSafe(size_t count, bool initNewElements = false, const Type* valueForNewElements = nullptr)
	{
		try
		{
			if (initNewElements)
			{
				if (!valueForNewElements)
				{
					ccLog::Warning("[ccArray::resizeSafe] Internal error: no new element specified");
					return false;
				}
				this->resize(count, *valueForNewElements);
			}
			else
			{
				this->resize(count);
			}
		}
		catch (const std::bad_alloc&)
		{
			return false;
		}
		return true;
	}

	// inherited from ccHObject
	/**
	 * @brief Get class type.
	 */
	inline virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::ARRAY;
	}

	/**
	 * @brief Check if shareable.
	 */
	inline virtual bool isShareable() const override
	{
		return true;
	}

	/**
	 * @brief Check if serializable.
	 */
	inline virtual bool isSerializable() const override
	{
		return true;
	}

	// Shortcuts
	/**
	 * @brief Get element at index.
	 */
	inline Type& getValue(size_t index)
	{
		return this->at(index);
	}

	/**
	 * @brief Get element at index (const).
	 */
	inline const Type& getValue(size_t index) const
	{
		return this->at(index);
	}

	/**
	 * @brief Set element at index.
	 */
	inline void setValue(size_t index, const Type& value)
	{
		this->at(index) = value;
	}

	/**
	 * @brief Add element to array.
	 */
	inline void addElement(const Type& value)
	{
		this->emplace_back(value);
	}

	/**
	 * @brief Fill array with value.
	 */
	inline void fill(const Type& value)
	{
		if (this->empty())
			this->resize(this->capacity(), value);
		else
			std::fill(this->begin(), this->end(), value);
	}

	/**
	 * @brief Get current size.
	 */
	inline unsigned currentSize() const
	{
		return static_cast<unsigned>(this->size());
	}

	/**
	 * @brief Clear array.
	 *
	 * @param[in] releaseMemory Release memory if true.
	 */
	inline void clear(bool releaseMemory = false)
	{
		if (releaseMemory)
			this->resize(0);
		else
			this->std::vector<Type>::clear();
	}

	/**
	 * @brief Swap two elements.
	 */
	inline void swap(size_t i1, size_t i2)
	{
		std::swap(this->at(i1), this->at(i2));
	}

  protected:
	/**
	 * @brief Destructor.
	 *
	 * @note Use release() instead.
	 */
	virtual ~ccArray()
	{
	}

	// Serialization
	/**
	 * @brief Serialize to file.
	 */
	inline bool toFile_MeOnly(QFile& out, short dataVersion) const override
	{
		return ccSerializationHelper::GenericArrayToFile<Type, N, ComponentType>(*this, out);
	}

	/**
	 * @brief Deserialize from file.
	 */
	inline bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override
	{
		return ccSerializationHelper::GenericArrayFromFile<Type, N, ComponentType>(*this, in, dataVersion, "array");
	}
};

#endif // CC_ARRAY_HEADER
