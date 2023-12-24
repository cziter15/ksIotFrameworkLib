/*
 *	Original author: Paul Varcholik
 *	Additional authors: Shao Voon Wong, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <stddef.h>

namespace ksf
{
	class ksRtti
	{
		public:
			/*
				@brief This function is used to get type ID of object.
				@return Type ID.
			*/
			virtual const size_t getInstanceType() const = 0;

			/*
				@brief This function is used to check if object is of given type.
				@param id Type ID.
				@return True if object is of given type, otherwise false.
			*/ 
			virtual bool isA(const size_t id) const
			{
				return false;
			}

			/*
				@brief This function is used to cast object to given type.
				@return Pointer to object of given type or nullptr if object is not of given type.
			*/
			template <typename TType>
			TType* as() 
			{
				if (is(TType::getClassType()))
					return (TType*)this;

				return nullptr;
			}

			/*
				@brief This function is used to const cast object to given type.
				@return Const pointer to object of given type or nullptr if object is not of given type.
			*/
			template <typename TType>
			const TType* as() const
			{
				if (is(TType::getClassType()))
					return (TType*)this;

				return nullptr;
			}
	};

	#define KSF_RTTI_DECLARATIONS(_Type, _ParentType)								\
		public:																		\
			virtual const size_t getInstanceType() const							\
			{																		\
				return _Type::getClassType(); 										\
			}																		\
			static const size_t getClassType()										\
			{																		\
				static int d{0}; return (size_t)&d; 								\
			}																		\
			virtual bool isA(const size_t id) const									\
			{																		\
				if (id == getClassType())											\
					return true;													\
				else																\
					return _ParentType::isA(id);									\
			}																		\
		private:
}