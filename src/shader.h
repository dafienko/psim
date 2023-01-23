#include <GL/glew.h>
#include <unordered_map>

class ShaderProgram {
	private:
		const std::unordered_map<std::string, GLuint> uniform_locations;
		GLuint program;
		GLuint vertex_shader, fragment_shader;
	public:
		ShaderProgram(const std::string &vs_filename, const std::string &fs_filename);
		~ShaderProgram();
		
		GLuint GetProgram();		
};