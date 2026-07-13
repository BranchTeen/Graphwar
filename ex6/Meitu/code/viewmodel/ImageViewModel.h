
#ifndef __IMAGE_VIEW_MODEL_H__
#define __IMAGE_VIEW_MODEL_H__

#include "../model/ImageModel.h"

#include "commands/LoadCommand.h"
#include "commands/GrayCommand.h"

class ImageViewModel : public IPropertyNotification,
			public LoadCommand<ImageViewModel>,
			public GrayCommand<ImageViewModel>
{
public:
	ImageViewModel() noexcept;
	ImageViewModel(const ImageViewModel&) = delete;
	~ImageViewModel() noexcept;

	ImageViewModel& operator=(const ImageViewModel&) = delete;

//properties
	const Fl_RGB_Image*const* get_image() const noexcept
	{
		return &m_pimage;
	}

//commands
	ICommandBase* get_load_command() noexcept
	{
		return static_cast<LoadCommand<ImageViewModel> *>(this);
	}
	ICommandBase* get_gray_command() noexcept
	{
		return static_cast<GrayCommand<ImageViewModel> *>(this);
	}

//trigger
	PropertyTrigger& get_trigger() noexcept
	{
		return m_trigger;
	}

//notification
	IPropertyNotification* get_notification() noexcept
	{
		return static_cast<IPropertyNotification *>(this);
	}

//methods
	void set_image_model(ImageModel* m)
	{
		m_image_model = m;
	}
	ImageModel* get_image_model() const noexcept
	{
		return m_image_model;
	}

private:
//overriders
	void on_property_changed(uint32_t id) override;

//tools
	void convert_image_data(const ImageDataT<ColorPixel> *pd);

private:
	//model
	ImageModel *m_image_model;

	//properties
	Fl_RGB_Image *m_pimage;

	//trigger
	PropertyTrigger m_trigger;
};

#endif
