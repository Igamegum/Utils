#include <fstream>
#include <tinyxml2.h>
#include <util/xmlpack.h>


bool ReadXMLNode(const std::string filename,const std::string attribute,std::string & text) {
	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename.c_str());

	tinyxml2::XMLElement * root_node = doc.RootElement();
	tinyxml2::XMLElement * string_node = root_node->FirstChildElement("string");

	while (string_node != nullptr) {
		const tinyxml2::XMLAttribute * attribute_of_string = string_node->FirstAttribute();
		if (attribute_of_string->Value() == attribute) {
			text = string_node->GetText();
			return true;
		}
		string_node = string_node->NextSiblingElement();
	}

	//Can not find this node
	return false;
}
bool SetXMLNode(const std::string filename,const std::string attribute,const std::string content) {
	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename.c_str()) ;

	tinyxml2::XMLElement * root_node = doc.RootElement();
	tinyxml2::XMLElement * string_node = root_node->FirstChildElement("string");
	bool exist = false;

	while (string_node != nullptr) {
		const tinyxml2::XMLAttribute * attribute_of_string = string_node->FirstAttribute();
		if (attribute_of_string->Value() == attribute) {
			exist = true;
			string_node->SetText(content.c_str());
			return doc.SaveFile(filename.c_str()) == 0;
		}
		string_node = string_node->NextSiblingElement();
	}

	//Can not find this node need to add a new node 
	if (exist == false) {

		tinyxml2::XMLElement * new_node = doc.NewElement("string");
		new_node->SetAttribute("name",attribute.c_str());

		tinyxml2::XMLText * content_text = doc.NewText(content.c_str());
		new_node->InsertEndChild(content_text);

		root_node->InsertEndChild(new_node);

		return  doc.SaveFile(filename.c_str()) == 0;
	}

	return false;
}

