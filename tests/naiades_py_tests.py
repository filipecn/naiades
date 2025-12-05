import numpy as np
import sys

sys.path.insert(0, "/home/filipecn/dev/naiades/build/Release/lib/")
import naiades_py


vertex_count = 10
vertices = np.empty(vertex_count * 3, dtype=np.float32)

result = naiades_py.sum_array(vertices)
print(f"Sum from C++: {result}")
