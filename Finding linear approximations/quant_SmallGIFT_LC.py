import time
from qiskit import IBMQ, assemble, transpile
from qiskit.tools.monitor import job_monitor
from qiskit import *
from qiskit.providers.aer import AerSimulator
from qiskit import QuantumRegister, ClassicalRegister, Aer, BasicAer, QuantumCircuit, execute
# from qiskit.visualization import plot_histogram
# from qiskit.providers.aer import QasmSimulator,StatevectorSimulator
# from qiskit.visualization import plot_state_city,circuit_drawer
# from qiskit.quantum_info import Statevector
import numpy as np

#TOKEN="5c68361d5b9e2c398434de7d981228d3556831658cded8c26a033c8ac53da6315d5323659c57a779fe4630f4fd9d3104ad65483f905c39767a8c8d30caa5a94f"
#IBMQ.save_account(TOKEN)
#IBMQ.load_account() # Load account from disk
# IBMQ.providers() 


def smallGIFT(block_size, key_size):
  qreg = QuantumRegister(block_size, 'q')
  circuit = QuantumCircuit(qreg, name="smallGIFT")

  #############First S-box#################
  circuit.ccx(qreg[0], qreg[2], qreg[1])
  circuit.ccx(qreg[3], qreg[1], qreg[0])
  circuit.x(qreg[0])
  circuit.x(qreg[1])
  circuit.ccx(qreg[0], qreg[1], qreg[2])
  circuit.x(qreg[0])
  circuit.x(qreg[1])
  circuit.x(qreg[2])
  circuit.cx(qreg[2], qreg[3])
  circuit.x(qreg[3])
  circuit.cx(qreg[3], qreg[1])
  circuit.x(qreg[1])
  circuit.ccx(qreg[0], qreg[1], qreg[2])
  circuit.swap(qreg[0],qreg[3])

  #############Second S-box#################
  circuit.ccx(qreg[4+0], qreg[4+2], qreg[4+1])
  circuit.ccx(qreg[4+3], qreg[4+1], qreg[4+0])
  circuit.x(qreg[4+0])
  circuit.x(qreg[4+1])
  circuit.ccx(qreg[4+0], qreg[4+1], qreg[4+2])
  circuit.x(qreg[4+0])
  circuit.x(qreg[4+1])
  circuit.x(qreg[4+2])
  circuit.cx(qreg[4+2], qreg[4+3])
  circuit.x(qreg[4+3])
  circuit.cx(qreg[4+3], qreg[4+1])
  circuit.x(qreg[4+1])
  circuit.ccx(qreg[4+0], qreg[4+1], qreg[4+2])
  circuit.swap(qreg[4+0],qreg[4+3])

  #############Permutation#################
  circuit.swap(qreg[1], qreg[3])
  circuit.swap(qreg[1], qreg[7])
  circuit.swap(qreg[2], qreg[5])
  circuit.swap(qreg[2], qreg[4])

  # #############Key Addition#################
  # circuit.cx(qreg[8],qreg[0])
  # circuit.cx(qreg[9],qreg[4])
  # circuit.cx(qreg[10],qreg[1])
  # circuit.cx(qreg[11],qreg[5])

  return circuit


def oracle_smallGIFT(block_size):
  qreg = QuantumRegister((block_size)*2, 'q')
  offset=block_size
  circuit = QuantumCircuit(qreg, name="oracle_smallGIFT_distribution")

  for i in range(offset):
    circuit.h(i)
    circuit.cx(i,offset+i)

  circuit.append(smallGIFT(block_size, key_size),range(offset))

  return circuit
  # the circuit now is F(x), x


start_time=time.time()
block_size=8
key_size=4
qreg_c = QuantumRegister(block_size*2, 'c')
qreg_z = QuantumRegister(block_size*2, 'z')
qreg_t = QuantumRegister(1, 'target')

creg_c = ClassicalRegister(block_size*2, 'c_m')
creg_t = ClassicalRegister(1, 'target_m')
circuit = QuantumCircuit(qreg_c,qreg_z, qreg_t, creg_c, creg_t)


# all C in superposition
circuit.h(range(block_size*2))

# superposition over subspace of Z
circuit.append(oracle_smallGIFT(block_size), range(block_size*2, block_size*4))

for i in range(block_size*2):
  circuit.ccx(qreg_c[i], qreg_z[i], qreg_t)

for i in range(block_size*2):
  circuit.measure(qreg_c[i], creg_c[i])

circuit.measure(qreg_t, creg_t)

# circuit_drawer(circuit, output='mpl')
# circuit.decompose(gates_to_decompose='oracle_smallGIFT_distribution').draw(output='mpl')
# the structure now is.... |c> |F(x)> |x>

#provider=IBMQ.get_provider('ibm-q')
#backend = provider.get_backend('simulator_mps')

#transpiled = transpile(circuit, backend=backend)

#job = execute(transpiled, backend, shots=10000);

simulator = AerSimulator(method='matrix_product_state')  

# Run and get counts, using the matrix_product_state method
tcirc = transpile(circuit, simulator)
result = simulator.run(tcirc, shots=70000).result()
counts = result.get_counts(0)


#job_monitor(job)

#result = job.result()
#counts = result.get_counts()
print(counts)

print("time taken in sec:", time.time()-start_time)

