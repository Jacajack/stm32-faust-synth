#ifndef FAUST_DSP_HPP
#define FAUST_DSP_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <initializer_list>
#include <cstring.hpp>

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

/**
	\brief Represents DSP control parameter
*/
struct faust_control
{
	std::string name;
	float *ptr;
	float min, max, def, step;
	std::unordered_map<cstring, cstring> metadata;
	enum
	{
		CONTROL_SLIDER,
		CONTROL_BUTTON,
		CONTROL_ENTRY
	} type;
};

/**
	\brief Parent class of fasut_dsp_* classes
	Provides Metadata and UI handling capabilities
*/
class faust_dsp_base
{	
protected:
	/**
		\brief Only used for redirecting metadata calls back to DSP class
	*/
	class Meta
	{
	public:
		const std::unordered_map<cstring, cstring> &get_metadata( ) const
		{
			return m_metadata;
		}
		
		void declare( const char *key, const char *value )
		{
			m_metadata[key] = value;
		}
		
	private:
		std::unordered_map<cstring, cstring> m_metadata;
	};

	/**
		\brief Only used for redirecting Faust's UI calls back to DSP class
	*/
	class UI
	{
	public:
		const std::unordered_map<float*, faust_control> &get_controls( ) const
		{
			return m_controls;
		}

		void openVerticalBox( const char *name )
		{
		}
		
		void openHorizontalBox( const char *name )
		{
		}
		
		void closeBox( )
		{
		}
		
		void addButton( const char *name, float *ptr )
		{
			auto &ctl = m_controls[ptr];
			ctl.name = name;
			ctl.ptr  = ptr;
			ctl.min  = 0;
			ctl.max  = 1;
			ctl.def  = 0;
			ctl.step = 1;
			ctl.type = faust_control::CONTROL_BUTTON;
		}
		
		void addCheckbox( const char *name, float *ptr )
		{
			auto &ctl = m_controls[ptr];
			ctl.name = name;
			ctl.ptr  = ptr;
			ctl.min  = 0;
			ctl.max  = 1;
			ctl.def  = 0;
			ctl.step = 1;
			ctl.type = faust_control::CONTROL_BUTTON;
		}
		
		void addHorizontalSlider( const char *name, float *ptr, float def, float min, float max, float step )
		{
			auto &ctl = m_controls[ptr];
			ctl.name = name;
			ctl.ptr  = ptr;
			ctl.min  = min;
			ctl.max  = max;
			ctl.def  = def;
			ctl.step = step;
			ctl.type = faust_control::CONTROL_SLIDER;
		}
		
		void addVerticalSlider( const char *name, float *ptr, float def, float min, float max, float step )
		{
			auto &ctl = m_controls[ptr];
			ctl.name = name;
			ctl.ptr  = ptr;
			ctl.min  = min;
			ctl.max  = max;
			ctl.def  = def;
			ctl.step = step;
			ctl.type = faust_control::CONTROL_SLIDER;
		}
		
		void addNumEntry( const char *name, float *ptr, float def, float min, float max, float step )
		{
			auto &ctl = m_controls[ptr];
			ctl.name = name;
			ctl.ptr  = ptr;
			ctl.min  = min;
			ctl.max  = max;
			ctl.def  = def;
			ctl.step = step;
			ctl.type = faust_control::CONTROL_ENTRY;
		}
		
		void declare( float *ptr, const char *key, const char *value )
		{
			m_controls[ptr].metadata[key] = value;
		}
		
	private:
		std::unordered_map<float*, faust_control> m_controls;
	};
		
public:
	/**
		Initializes DSP's interface and returns a map of control parameters (name -> parameter)
	*/
	std::unordered_map<float*, faust_control> init_ui( )
	{
		UI ui;
		buildUserInterface( &ui );
		return ui.get_controls( );
	}
	
	/**
		Collects metadata from the DSP and returns it in a map
	*/
	std::unordered_map<cstring, cstring> init_metadata(  )
	{
		Meta meta;
		metadata( &meta );
		return meta.get_metadata( );
	}
	
	virtual void metadata( Meta* m ) = 0;
	virtual int getNumInputs( ) = 0;
	virtual int getNumOutputs( ) = 0;
	virtual int getInputRate( int channel ) {return getSampleRate();}
	virtual int getOutputRate( int channel ) {return getSampleRate();}
	virtual void instanceConstants( int samplingFreq ) = 0;
	virtual void instanceResetUserInterface( ) = 0;
	virtual void instanceClear( ) = 0;
	virtual void init( int samplingFreq ) = 0;
	virtual void instanceInit( int samplingFreq ) = 0;
	virtual int getSampleRate( ) = 0;	
	virtual void buildUserInterface( UI* ui_interface ) = 0;
	virtual void compute( int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs ) = 0;
};


/**
	A wrapper class for faust_dsp_base. We don't need so many exposed functions and stuff.
	Also, this class manages initialization of the DSP
	\todo add reset functions
*/
class faust_dsp
{
public:
	/**
		Performs DSP initialization and reads metadata and controls from the DSP
	*/
	faust_dsp( faust_dsp_base *dsp, int samplerate ) :
		m_dsp( dsp )
	{
		// Initialize the DSP
		m_dsp->init( samplerate );
		
		// Get DSP controls
		m_controls = m_dsp->init_ui( );
		
		// Gather metadata
		m_metadata = m_dsp->init_metadata( );
	}
	
	unsigned int get_input_count( ) {return m_dsp->getNumInputs( );}
	unsigned int get_output_count( ) {return m_dsp->getNumOutputs( );}
	int get_sample_rate( ) {return m_dsp->getSampleRate( );}
	int get_input_rate( int channel ) {return m_dsp->getInputRate( channel );}
	int get_output_rate( int channel ) {return m_dsp->getOutputRate( channel );}
	
	void compute( int count, FAUSTFLOAT **inputs, FAUSTFLOAT **outputs )
	{
		m_dsp->compute( count, inputs, outputs );
	}
	
	//! This version in essentially only syntatic sugar, but it's probably slower
	void compute( int count, std::initializer_list<FAUSTFLOAT*> inputs, std::initializer_list<FAUSTFLOAT*> outputs )
	{
		if ( inputs.size( ) != get_input_count( ) || outputs.size( ) != get_output_count( ) )
			throw std::runtime_error( "DSP input/output count mismatch" );
		m_dsp->compute( count, const_cast<FAUSTFLOAT**>( inputs.begin( ) ), const_cast<FAUSTFLOAT**>( outputs.begin( ) ) );
	}
	
	const faust_control *get_control_by_name( const cstring &name )
	{
		for ( const auto &[ptr, ctl] : m_controls )
		{
			if ( ctl.name == name.c_str( ) )
				return &ctl;
		}

		return nullptr;
	}

	// Metadata and controls access
	const std::unordered_map<float*, faust_control> &get_controls( ) const {return m_controls;}
	const std::unordered_map<cstring, cstring> &get_metadata( ) const {return m_metadata;}
	
private:
	std::unique_ptr<faust_dsp_base> m_dsp;
	std::unordered_map<float*, faust_control> m_controls;
	std::unordered_map<cstring, cstring> m_metadata;
};



#endif
