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
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef CC_GRAPHICAL_TRANSFORMATION_TOOL_HEADER
#define CC_GRAPHICAL_TRANSFORMATION_TOOL_HEADER

/**
 * @file ccGraphicalTransformationTool.h
 *
 * @brief Graphical transformation tool for interactive entity manipulation.
 *
 * @details Provides an interactive overlay dialog for transforming entities
 * (point clouds, meshes, etc.) using mouse gestures and UI controls.
 *
 * Features:
 * - Mouse-driven rotation and translation in 3D view
 * - Rotation around arbitrary axes
 * - Translation along arbitrary vectors
 * - Advanced mode with reference point/axis selection
 * - Real-time preview of transformations
 * - Apply/cancel workflow
 *
 * The tool displays the transformation matrix and allows fine control
 * over translation and rotation parameters.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccOverlayDialog
 * @see ccGLMatrixd
 */

// Local
#include <ccOverlayDialog.h>
#include <ui_graphicalTransformationDlg.h>

// qCC_db
#include <ccHObject.h>

class ccGLWindowInterface;

/**
 * @brief Graphical transformation tool for interactive entity manipulation.
 *
 * @details An overlay dialog that provides interactive transformation
 * controls for selected entities. Users can:
 * - Drag entities in the 3D view to translate them
 * - Use rotation handles to rotate around axes
 * - Enter precise translation/rotation values
 * - Use advanced mode with custom reference points/axes
 *
 * The tool maintains a transformation matrix that is applied to
 * selected entities. Transformations can be applied incrementally
 * or all at once.
 *
 * @extends ccOverlayDialog
 * @extends Ui::GraphicalTransformationDlg
 */
class ccGraphicalTransformationTool : public ccOverlayDialog
    , public Ui::GraphicalTransformationDlg
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the transformation tool.
	 *
	 * @param[in] parent Parent widget.
	 */
	explicit ccGraphicalTransformationTool(QWidget* parent);

	/**
	 * @brief Destructor.
	 */
	virtual ~ccGraphicalTransformationTool();

	// inherited from ccOverlayDialog
	/**
	 * @brief Link the tool with a 3D view.
	 *
	 * @param[in] win Window to link with.
	 * @return true if linking succeeded.
	 */
	virtual bool linkWith(ccGLWindowInterface* win) override;

	/**
	 * @brief Start the transformation mode.
	 *
	 * @return true if started successfully.
	 */
	virtual bool start() override;

	/**
	 * @brief Stop the transformation mode.
	 *
	 * @param[in] state Final state to apply.
	 */
	virtual void stop(bool state) override;

	/**
	 * @brief Clear advanced mode reference entities.
	 */
	void clearAdvModeEntities();

	/**
	 * @brief Add an entity to the transformation set.
	 *
	 * @param[in] anObject Entity to add.
	 * @return true if the entity is eligible for transformation.
	 *
	 * @details Only entities in the "to transform" set will be
	 * moved when the transformation is applied.
	 */
	bool addEntity(ccHObject* anObject);

	/**
	 * @brief Get the number of valid entities.
	 *
	 * @return Number of entities in the transformation set.
	 */
	unsigned getNumberOfValidEntities() const;

	/**
	 * @brief Get the entities to be transformed.
	 *
	 * @return Reference to the container of entities.
	 */
	const ccHObject& getValidEntities() const
	{
		return m_toTransform;
	}

	/**
	 * @brief Set the rotation center.
	 *
	 * @param[in] center New rotation center.
	 */
	void setRotationCenter(CCVector3d& center);

	/**
	 * @brief Get transformation for arbitrary vector translation.
	 *
	 * @param[in] vec Translation vector.
	 * @return Transformation matrix.
	 */
	ccGLMatrixd arbitraryVectorTranslation(const CCVector3& vec);

	/**
	 * @brief Get transformation for arbitrary vector rotation.
	 *
	 * @param[in] angle Rotation angle in radians.
	 * @param[in] vec Rotation axis vector.
	 * @return Transformation matrix.
	 */
	ccGLMatrixd arbitraryVectorRotation(double angle, const CCVector3d&);

  protected:
	/**
	 * @brief Rotation axis selection enum.
	 */
	enum rotComboBoxItems
	{
		XYZ,   //!< Rotate around all three axes
		X,     //!< Rotate around X axis
		Y,     //!< Rotate around Y axis
		Z,     //!< Rotate around Z axis
		NONE   //!< No rotation
	};

	/**
	 * @brief Apply the current transformation to entities.
	 */
	void apply();

	/**
	 * @brief Reset transformation to identity.
	 */
	void reset();

	/**
	 * @brief Cancel without applying transformation.
	 */
	void cancel();

	/**
	 * @brief Pause/resume transformation mode.
	 *
	 * @param[in] state true to pause, false to resume.
	 */
	void pause(bool state);

	/**
	 * @brief Toggle advanced mode UI visibility.
	 *
	 * @param[in] state true to enable advanced mode.
	 */
	void advModeToggle(bool state);

	/**
	 * @brief Update transform when translate reference changes.
	 *
	 * @param[in] index Reference index.
	 */
	void advTranslateRefUpdate(int index);

	/**
	 * @brief Update transform when rotate reference changes.
	 *
	 * @param[in] index Reference index.
	 */
	void advRotateRefUpdate(int index);

	/**
	 * @brief Update rotation combo box selection.
	 *
	 * @param[in] index Selected rotation axis.
	 */
	void advRotateComboBoxUpdate(int index);

	/**
	 * @brief Toggle rotation center to reference object.
	 *
	 * @param[in] state Radio button state.
	 */
	void advRefAxisRadioToggled(bool state);

	/**
	 * @brief Toggle incremental translation fields.
	 */
	void incrementalTranslationToggle();

	/**
	 * @brief Toggle incremental rotation fields.
	 *
	 * @param[in] selectedRotationItem Current rotation item.
	 */
	void incrementalRotationToggle(const rotComboBoxItems& selectedRotationItem);

	/**
	 * @brief Toggle rotation center to object center.
	 *
	 * @param[in] state Radio button state.
	 */
	void advObjectAxisRadioToggled(bool state);

	/**
	 * @brief Update the display message based on mode.
	 */
	void updateDisplayMessage();

	/**
	 * @brief Apply graphical translation to selected entities.
	 *
	 * @param[in] Translation vector.
	 */
	void glTranslate(const CCVector3d&);

	/**
	 * @brief Apply graphical rotation to selected entities.
	 *
	 * @param[in] rot Rotation matrix.
	 */
	void glRotate(const ccGLMatrixd&);

	/**
	 * @brief Apply incremental transform from spin boxes.
	 *
	 * @param[in] forward true to apply forward, false to reverse.
	 */
	void incrementalTransform(bool forward = true);

	/**
	 * @brief Handle keyboard shortcut triggers.
	 *
	 * @param[in] id Shortcut identifier.
	 */
	void onShortcutTriggered(int);

	/**
	 * @brief Clear all state and unlink.
	 */
	void clear();

	/**
	 * @brief Update GL transformation matrices.
	 */
	void updateAllGLTransformations();

	/**
	 * @brief Populate advanced mode item list.
	 */
	void populateAdvModeItems();

	/**
	 * @brief Set advanced translate reference transform.
	 *
	 * @param[in] translateRef Reference entity.
	 * @return true if set successfully.
	 */
	bool setAdvTranslationTransform(ccHObject* translateRef);

	/**
	 * @brief Set advanced rotation axis.
	 *
	 * @param[in] rotateRef Reference entity.
	 * @param[in] selectedAxis Selected rotation axis.
	 * @return true if set successfully.
	 */
	bool setAdvRotationAxis(ccHObject* rotateRef, rotComboBoxItems selectedAxis);

	/**
	 * @brief Check if entity is in transformation list.
	 *
	 * @param[in] entity Entity to check.
	 * @return true if entity is being transformed.
	 */
	bool entityInTransformList(ccHObject* entity);

	//! Advanced mode flag
	bool m_advMode;

	//! Flag if rotation reference is a child of entities
	bool m_advRotateRefIsChild;

	//! Flag if translate reference is a child of entities
	bool m_advTranslateRefIsChild;

	//! Container of entities to transform
	ccHObject m_toTransform;

	//! Advanced mode translate reference entity
	ccHObject* m_advTranslateRef = nullptr;

	//! Advanced mode rotate reference entity
	ccHObject* m_advRotateRef = nullptr;

	//! Current rotation matrix
	ccGLMatrixd m_rotation;

	//! Current translation vector
	CCVector3d m_translation;

	//! Current position matrix
	ccGLMatrixd m_position;

	//! Advanced mode translation transform
	ccGLMatrixd m_advTranslationTransform;

	//! Advanced mode rotation axis vector
	CCVector3d m_advRotationAxis;

	//! Advanced mode rotation reference object center
	CCVector3 m_advRotationRefObjCenter;

	//! Rotation center (center of gravity of selected entities)
	CCVector3d m_rotationCenter;

	//! Advanced mode available objects list
	ccHObject::Container m_advancedModeObjectList;
};

#endif // CC_GRAPHICAL_TRANSFORMATION_TOOL_HEADER
