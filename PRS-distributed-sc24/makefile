# 定义源文件列表
SOURCES := $(shell find ./src -type f -name "*.cpp")

# 将源文件列表转换为目标文件列表
OBJECTS := $(addprefix build/,$(SOURCES:%=%.o))

# 声明编译器和编译选项
CXX := mpicxx
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -MMD -MP -march=native -mtune=native -g 

LIBS := -Wl,-Bstatic -lkissat -L kissat-inc/build -I kissat-inc/ -I lingeling \
	    -lmapleCOMSPS -L mapleCOMSPS/build/release/lib -I mapleCOMSPS/mapleCOMSPS/ \
	    -lm4ri -L mapleCOMSPS/m4ri-20140914/.libs -I mapleCOMSPS/m4ri-20140914/ \
		-L lingeling -l lgl -L yalsat -l yals \
        -pthread -lz -lm -lboost_thread -lboost_date_time -lboost_system \
		-Wl,-Bdynamic -lmpi_cxx -lmpi 

# 默认目标，编译所有目标文件
PRS-distributed: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

build/%.o: % | build
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LIBS)

# 包含依赖文件
-include $(OBJECTS:.o=.d)

# 如果需要，则递归创建源文件的目录结构
build:
	mkdir -p $(sort $(dir $(OBJECTS)))

# 清除所有目标文件和build目录
clean:
	rm -rf build $(OBJECTS)

.PHONY: clean all