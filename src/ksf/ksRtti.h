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

			template <typename _Type>
			_Type* as() 
			{
				if (is(_Type::getClassType()))
					return (_Type*)this;

				return nullptr;
			}
			template <typename _Type>
			const _Type* as() const
			{
				if (is(_Type::getClassType()))
					return (_Type*)this;

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
				static int d = 0; return (size_t) &d; 								\
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