#pragma once

#include "Hazel/Layer.h"

namespace Hazel
{
	// export 해줘야 한다.
	class HAZEL_API ImGuiLayer : public Layer
	{
		public:
			ImGuiLayer();
			~ImGuiLayer();

			void OnAttach() override;
			void OnDetach() override;
			void OnUpdate() override;
			void OnEvent(Event& event) override;
		private:
			float m_Time;
	};
}



