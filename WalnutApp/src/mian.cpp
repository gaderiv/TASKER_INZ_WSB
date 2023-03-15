#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"

enum TaskStatus
{
	ONGOING,
	CLOSED
};

//czerwony kolor ImVec4(1.0f, 0.0f, 0.0f, 1.0f)

//enum TaskPriority
//{
//	LOW,
//	MEDIUM,
//	HIGH
//};

struct Task
{
	std::string description;
	bool completed;
	TaskStatus status;
	//TaskPriority priority;
};

std::vector<Task> tasks;
std::vector<int> ongoingTasks;
std::vector<int> closedTasks;

static char Input[256] = "";
static bool TaskAdded = false;
static int selectedTask = -1; //index zaznaczonych taskow do edycji

static char username[256] = "";
static char password[256] = "";
static bool authenticated = false;
//static TaskPriority selectedPriority = TaskPriority::LOW;

//login
void LoginForm()
{
	static bool checker = true;

	ImGui::Begin("Login", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(ImVec2(400, 150));
	ImGui::SetWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) * 0.5f,
								(ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) * 0.5f));

	//ImGuiInputTextFlags_EnterReturnsTrue sprawdzanie wcisniecia klawisza enter
	ImGui::InputText("Username", username, 256, ImGuiInputTextFlags_EnterReturnsTrue);
	if (ImGui::InputText("Password", password, 256, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		//jesli uzytkownik kliknal enter zaloguj
		if (strcmp(username, "Admin") == 0 && strcmp(password, "admin") == 0)
		{
			authenticated = true;
		}
		else
		{
			authenticated = false;
			checker = false;
		}
	}

	if (ImGui::Button("Login") || authenticated)
	{
		if (strcmp(username, "Admin") == 0 && strcmp(password, "admin") == 0)
		{
			authenticated = true;
		}
		else
		{
			authenticated = false;
			checker = false;
		}
	}

	if (!checker) 
	{
		ImGui::Text("Invalid username or password");
	}
	ImGui::End();
}

//Funkcja odpowiedzialna za popup i edytowanie taskow
void Editor()
{
	if (selectedTask >= 0)
	{
		ImGui::OpenPopup("Edit Task");
		if (ImGui::BeginPopupModal("Edit Task", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Task Description", Input, sizeof(Input));

			//ImGui::Combo("Task Priority", (int*)&tasks[selectedTask].priority, "Low\0Medium\0High\0");

			if (ImGui::Button("Save"))
			{
				tasks[selectedTask].description = Input;
				Input[0] = '\0';
				selectedTask = -1;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				Input[0] = '\0';
				selectedTask = -1;
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::EndPopup();
	}
}
//status taskow
void StatusDashboard()
{
	ImGui::Begin("Status");

	//dashbord trwajace taski
	ImGui::BeginChild("Ongoing Tasks", ImVec2(0, 200), true);
	ImGui::Text("Ongoing Tasks (%d)", ongoingTasks.size());
	if (!TaskAdded)
	{
		for (int i = 0; i < ongoingTasks.size(); i++)
		{
			int index = ongoingTasks[i];
			Task& task = tasks[index];
			std::string taskLabel =  task.description;
			if (ImGui::Checkbox(taskLabel.c_str(), &task.completed))
			{
				//zmien status jak checkbox jest zaznaczony
				task.status = task.completed ? TaskStatus::CLOSED : TaskStatus::ONGOING;

				if (task.status == TaskStatus::CLOSED)
				{
					closedTasks.push_back(index);
					ongoingTasks.erase(ongoingTasks.begin() + i);
					i--;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(("Edit##" + std::to_string(index)).c_str()))
			{
				selectedTask = index;
				std::strcpy(Input, task.description.c_str());
			}
			ImGui::SameLine();
			if (ImGui::Button(("Delete##" + std::to_string(i)).c_str()))
			{
				tasks.erase(tasks.begin() + index);
				ongoingTasks.erase(ongoingTasks.begin() + i);
				for (int j = 0; j < ongoingTasks.size(); j++)
				{
					if (ongoingTasks[j] > index)
					{
						ongoingTasks[j]--;
					}
				}
				for (int j = 0; j < closedTasks.size(); j++)
				{
					if (closedTasks[j] > index)
					{
						closedTasks[j]--;
					}
				}
				i = -1; //restart petli
			}
		}
	}
	ImGui::EndChild();

	//dashboard zamkniete taski
	ImGui::BeginChild("Closed Tasks", ImVec2(0, 200), true);
	ImGui::Text("Closed Tasks (%d)", closedTasks.size());
	if (!TaskAdded)
	{
		for (int i = 0; i < closedTasks.size(); i++)
		{
			int index = closedTasks[i];
			Task& task = tasks[index];
			std::string taskLabel = task.description;
			if (ImGui::Checkbox(taskLabel.c_str(), &task.completed))
			{
				//zmien status jak checkbox jest zaznaczony
				task.status = task.completed ? TaskStatus::CLOSED : TaskStatus::ONGOING;

				if (task.status == TaskStatus::CLOSED)
				{
					closedTasks.push_back(index);
					for (int j = 0; j < ongoingTasks.size(); j++)
					{
						if (ongoingTasks[j] == index)
						{
							ongoingTasks.erase(ongoingTasks.begin() + j);
							break;
						}
					}
				}
				else
				{
					ongoingTasks.push_back(index);
					for (int j = 0; j < closedTasks.size(); j++)
					{
						if (closedTasks[j] == index)
						{
							closedTasks.erase(closedTasks.begin() + j);
							break;
						}
					}
				}
			}
			ImGui::SameLine();

			//funkcja odpowiedzialna za edytowanie w przypadku zamknietych wylaczona

			/*if (ImGui::Button(("Edit##" + std::to_string(index)).c_str()))
			{
				selectedTask = index;
				std::strcpy(Input, task.description.c_str());
			}
			ImGui::SameLine();*/


			if (ImGui::Button(("Delete##" + std::to_string(index)).c_str()))
			{
				if (tasks[index].status == TaskStatus::ONGOING) 
				{
					for (int j = 0; j < ongoingTasks.size(); j++) 
					{
						if (ongoingTasks[j] == index) 
						{
							ongoingTasks.erase(ongoingTasks.begin() + j);
							break;
						}
					}
				}
				else 
				{
					for (int j = 0; j < closedTasks.size(); j++) 
					{
						if (closedTasks[j] == index) 
						{
							closedTasks.erase(closedTasks.begin() + j);
							break;
						}
					}
				}
				tasks.erase(tasks.begin() + index);
				for (int j = 0; j < ongoingTasks.size(); j++) 
				{
					if (ongoingTasks[j] > index) 
					{
						ongoingTasks[j]--;
					}
				}
				for (int j = 0; j < closedTasks.size(); j++) 
				{
					if (closedTasks[j] > index) 
					{
						closedTasks[j]--;
					}
				}
				i = -1; //restart petli
			}
		}
	}
	
	ImGui::EndChild();

	ImGui::End();
}
//dodawanie taskow
void taskAddeer()
{
	ImGui::Begin("Tasker");
	ImGui::Text("Task Description");
	ImGui::SameLine();
	ImGui::InputText("##Task Description", Input, sizeof(Input));
	ImGui::SameLine();
	//ImGui::Combo("Task Priority", (int*)&selectedPriority, "Low\0Medium\0High\0");

	if (ImGui::Button("Add Task", ImVec2(150.0f, 0.0f)) && Input[0] != '\0' && !std::isspace(Input[0]))
	{
		tasks.push_back({ Input, false, TaskStatus::ONGOING });
		Input[0] = '\0';
		TaskAdded = true;

		if (tasks.back().status == TaskStatus::ONGOING)
		{
			ongoingTasks.push_back(tasks.size() - 1);
		}
		else
		{
			closedTasks.push_back(tasks.size() - 1);
		}

	}
	else
	{
		TaskAdded = false;
	}

	ImGui::End();
}

void theme()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowBorderSize = 0;
	style->WindowTitleAlign = ImVec2(0.5, 0.5);
	style->WindowMinSize = ImVec2(200, 400);

	style->FramePadding = ImVec2(8, 6);

	style->Colors[ImGuiCol_TitleBg] = ImColor(255, 101, 53, 255);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(255, 101, 53, 255);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 130);

	style->Colors[ImGuiCol_Button] = ImColor(31, 30, 31, 255);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(41, 40, 41, 255);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(41, 40, 41, 255);

	style->Colors[ImGuiCol_Separator] = ImColor(70, 70, 70, 255);
	style->Colors[ImGuiCol_SeparatorActive] = ImColor(76, 76, 76, 255);
	style->Colors[ImGuiCol_SeparatorHovered] = ImColor(76, 76, 76, 255);

	style->Colors[ImGuiCol_FrameBg] = ImColor(37, 36, 37, 255);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(37, 36, 37, 255);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(37, 36, 37, 255);

	style->Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(46, 46, 46, 255);
}


class ExampleLayer : public Walnut::Layer
{
public:
	
	virtual void OnUIRender() override
	{
		theme();
		if (!authenticated) 
		{
			LoginForm();
			return;
		}

		taskAddeer();
		Editor();
		StatusDashboard();



		//ImGui::ShowDemoWindow();
	}
};



Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Tasker_Inz_WSB";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
		if (authenticated)
		{
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Right-aligned text").x);
			if (ImGui::BeginMenu("##Account"))
			{
				if (ImGui::MenuItem("Logut"))
				{
					authenticated = false;
				}
				ImGui::EndMenu();
			}
		}
		

	// dodaj username na pasku menu
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Right-aligned text").x);
	if (authenticated)
	{
		ImGui::Text("Welcome, %s", username); 
	}

		
	});
	return app;
}