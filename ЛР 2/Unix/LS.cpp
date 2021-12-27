#include <iostream>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>

char* permissions(struct stat st) {
	char* modeval = (char*)malloc(sizeof(char) * 11 + 1);
	mode_t perm = st.st_mode;
	modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
	modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
	modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
	modeval[3] = ' ';
	modeval[4] = (perm & S_IRGRP) ? 'r' : '-';
	modeval[5] = (perm & S_IWGRP) ? 'w' : '-';
	modeval[6] = (perm & S_IXGRP) ? 'x' : '-';
	modeval[7] = ' ';
	modeval[8] = (perm & S_IROTH) ? 'r' : '-';
	modeval[9] = (perm & S_IWOTH) ? 'w' : '-';
	modeval[10] = (perm & S_IXOTH) ? 'x' : '-';
	modeval[11] = '\0';
	return modeval;
}

std::vector<std::string> get_file_data(struct stat file_info, struct dirent* entry_info) {
	std::vector<std::string> file_importants;
	std::string name = entry_info->d_name;
	size_t dotID;
	char* size = (char*)malloc(sizeof(unsigned long long));
	char* type = (char*)malloc(sizeof entry_info->d_name);
	char* userid = (char*)malloc(sizeof(unsigned long long));
	struct timespec ts = file_info.st_atim;
	char* atime = (char*)calloc(200, sizeof(char));
	char* time = (char*)calloc(300, sizeof(char));

	file_importants.push_back(entry_info->d_name);//name
	if (S_ISREG(file_info.st_mode)) {
		sprintf(size, "%ld", file_info.st_size);
		file_importants.push_back(std::string(size));//size
		try {
			dotID = name.find_last_of('.');
			name.copy(type, name.size() - dotID, dotID);
			file_importants.push_back(type);//type
		}
		catch(std::out_of_range){
			file_importants.push_back("<TYPE>");
		}
	}
	else if (S_ISDIR(file_info.st_mode)) {
		file_importants.push_back("");//size
		file_importants.push_back("DIR");//type
	}
	else if(S_ISLNK(file_info.st_mode)){
		file_importants.push_back("");//size
		file_importants.push_back("LINK");//type
	}
	else {
		file_importants.push_back("");//size
		file_importants.push_back("<TYPE>");//type
	}
	sprintf(userid, "%u", file_info.st_uid);
	file_importants.push_back(userid); //userid
	strftime(atime, 200, "%D %T", gmtime(&ts.tv_sec));
	sprintf(time, "%s.%09ld UTC", atime, ts.tv_nsec);
	file_importants.push_back(time); //last access
	file_importants.push_back(permissions(file_info)); //permissions
	return file_importants;
}

void lsprint(std::vector<std::string> dir_data) {
	int i, j;
	unsigned int dir_list_size = dir_data.size();
	unsigned int cell_size;
	unsigned int size_diff;
	std::string cell = "";
	std::string delimiter = "+";
	std::vector<unsigned int> col_sizes(6, 0);
	unsigned int entry_num = dir_list_size / 6;
	for (i = 0; i < 6 ; i++) {
		for (j = 0; j < entry_num; j++) {
			cell_size = dir_data.at(6 * j + i).size();
			if (col_sizes[i] < cell_size) {
				col_sizes[i] = cell_size;
			}
		}
	}
	for (i = 0; i < 6; i++) {
		cell = std::string(col_sizes[i], '-');
		delimiter += cell + "+";		
	}
	printf("%s\n|", delimiter.c_str());
	for (i = 0; i < 6; i++) {
		cell = dir_data[i];
		size_diff = col_sizes[(i % 6)] - cell.size();
		cell.append(std::string(size_diff, ' '));
		std::cout << cell;
		printf("|");
	}
	cell = dir_data[6];
	size_diff = col_sizes[0] - cell.size();
	cell.append(std::string(size_diff, ' '));
	printf("\n%s\n|%s", delimiter.c_str(), cell.c_str());
	for (i = 7; i < dir_list_size; i++) {
		if ((i % 6) == 0) {
			printf("|\n|");
		}
		else {
			printf("|");
		}
		cell = dir_data[i];
		size_diff = col_sizes[(i % 6)] - cell.size();
		cell.append(std::string(size_diff, ' '));
		std::cout << cell;
	}
	printf("|\n%s\n", delimiter.c_str());
}

void ls(std::vector<std::string> pathVector) {
	unsigned int count = 0;
	std::vector<std::string> dir_data = {"Name","Size(b)","Type","User ID","Last access","Usr Grp Oth"};
	std::vector<std::string> file_importants;
	std::vector<std::string> empty_dirs;
	std::string path;
	DIR* direction;
	struct dirent* entry;
	struct stat file_info;
	for (int i = 0; i < pathVector.size(); i++) {
		count = 0;
		direction = opendir(pathVector[i].c_str());
		while((entry = readdir(direction)) != NULL){
			path = std::string(pathVector[i]) + std::string("/") + entry->d_name;
			lstat(path.c_str(), &file_info);
			file_importants = get_file_data(file_info, entry);
			dir_data.insert(dir_data.end(), file_importants.begin(), file_importants.end());
			count++;
		}
		if (count < 3) {
			empty_dirs.push_back(pathVector[i]);
		}
		closedir(direction);
		printf("Direction: %s\nEntries num: %u\n", pathVector[i].c_str(), count - 2);
		lsprint(dir_data);
		dir_data.resize(6);
	}
	if (empty_dirs.size() > 0) {
		printf("Empty directions:\n");
	}
	for (auto path: empty_dirs) {
		printf("-%s\n", path.c_str());
	}
}

int main() {
	std::vector<std::string> pathVector;
	std::string path;
	std::cout << "Enter set of paths:\n(use ',' between path)\n";
	std::cout << "> ";
	while (true) {
		std::cin >> path;
		pathVector.push_back(path);
		if (path[path.size() - 1] != ',') {
			break;
		}
		pathVector.back().pop_back();
		std::cout << "> ";
	}
	std::cout << "\n";
	ls(pathVector);
	exit(0); 
}