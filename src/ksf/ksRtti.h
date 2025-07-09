/*
 *	Original author: Paul Varcholik
 *	Additional authors: Shao Voon Wong, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <cstddef>

namespace ksf
{
	/*!
		@brief Implements RTTI (run-time type information) for objects.

		This is a simple, fast, and lightweight implementation of the RTTI feature. 
		It provides the ability to check whether an object is of a given type and to cast it to the type specified as a template parameter.

		It is extensively used to implement components (ksComponent) and to easily manage components within the application (ksApplication).
	*/
	class ksRtti
	{
		public:
			/*! 
				@brief  Destructor. 
			*/
			virtual ~ksRtti() = default;

			/*!
				@brief Retrieves type ID of the object.
				@return Object type ID.
			*/
			virtual const std::size_t getInstanceType() const = 0;

			/*!
				@brief Checks whether object is of given type.
				@param id Type ID to check against.
				@return True if object is of given type, otherwise false.
			*/ 
			virtual bool isA(const std::size_t id) const
			{
				return false;
			}

			/*!
				@brief Tries to cast object to the type provided as a template parameter.
				@tparam TType Target type.
				@return Pointer to object of given type or nullptr if object is not of given type.
			*/
			template <typename TType>
			TType* as() 
			{
				if (isA(TType::getClassType()))
					return static_cast<TType*>(this);

				return nullptr;
			}

			/*!
				@brief Tries to cast object to the type provided as a template parameter (const version).
				@tparam TType Target type.
				@return Const pointer to object of given type or nullptr if object is not of given type.
			*/
			template <typename TType>
			const TType* as() const
			{
				if (isA(TType::getClassType()))
					return static_cast<const TType*>(this);

				return nullptr;
			}
	};

	#define KSF_RTTI_DECLARATIONS(_Type, _ParentType)								\
		public:																		\
			virtual const std::size_t getInstanceType() const						\
			{																		\
				return _Type::getClassType(); 										\
			}																		\
			static const std::size_t getClassType()									\
			{																		\
				static int d{0}; return (std::size_t)&d; 							\
			}																		\
			virtual bool isA(const std::size_t id) const							\
			{																		\
				if (id == getClassType())											\
					return true;													\
				else																\
					return _ParentType::isA(id);									\
			}																		\
		private:
}