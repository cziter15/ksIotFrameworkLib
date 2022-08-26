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
			virtual const size_t getInstanceType() const = 0;

			virtual bool isA(const size_t id) const
			{
				return false;
			}

			template <typename T>
			T* as() 
			{
				if (is(T::getClassType()))
					return (T*)this;

				return nullptr;
			}
			template <typename T>
			const T* as() const
			{
				if (is(T::getClassType()))
					return (T*)this;

				return nullptr;
			}
	};

	#define KSF_RTTI_DECLARATIONS(Type, ParentType)									\
		public:																		\
			virtual const size_t getInstanceType() const							\
			{																		\
				return Type::getClassType(); 										\
			}																		\
			static const size_t getClassType()										\
			{																		\
				static int d = 0; return (size_t) &d; 								\
			}																		\
			virtual bool isA(const size_t id) const									\
			{																		\
				if (id == getClassType())											\
					return true;													\
				else																\
					return ParentType::isA(id);										\
			}																		\
		private:
}