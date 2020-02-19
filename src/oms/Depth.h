#pragma once
#include "types.h"

namespace OMS {

	class Depth
	{
	public:

		class Level {

		public:

		protected:

			Price price_;
			Quantity aggregated_quantity_;

		};

		Depth();
		virtual ~Depth();

	protected:
		const int size = 10;
	};

}