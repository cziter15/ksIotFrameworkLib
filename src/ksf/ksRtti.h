#pragma once

#include <stddef.h>

namespace ksf
{
	class ksRtti
	{
		public:
			virtual const size_t typeIdInstance() const = 0;
			
			virtual ksRtti* queryInterface(const size_t)
			{
				return nullptr;
			}

			virtual const ksRtti* queryInterface(const size_t) const
			{
				return nullptr;
			}

			virtual bool is(const size_t id) const
			{
				return false;
			}

			template <typename T>
			T* as() 
			{
				if (is(T::typeIdClass()))
				{
					return (T*)this;
				}

				return nullptr;
			}
			template <typename T>
			const T* as() const
			{
				if (is(T::typeIdClass()))
				{
					return (T*)this;
				}

				return nullptr;
			}
	};

	#define KS_RTTI_DECLARATIONS(Type, ParentType)									\
		public:																		\
			virtual const size_t typeIdInstance() const								\
			{																		\
				return Type::typeIdClass(); 										\
			}																		\
			static const size_t typeIdClass()										\
			{																		\
				static int d = 0; return (size_t) &d; 								\
			}																		\
			virtual ksf::ksRtti* queryInterface( const size_t id )					\
			{																		\
				if (id == typeIdClass())											\
					return (ksf::ksRtti*)this;											\
				else																\
					return ParentType::queryInterface(id);							\
			}																		\
			virtual const ksf::ksRtti* queryInterface( const size_t id ) const		\
			{																		\
				if (id == typeIdClass())											\
					return (ksf::ksRtti*)this;											\
				else																\
					return ParentType::queryInterface(id);							\
			}																		\
			virtual bool is(const size_t id) const									\
			{																		\
				if (id == typeIdClass())											\
					return true;													\
				else																\
					return ParentType::is(id);										\
			}																		\
		private:
}