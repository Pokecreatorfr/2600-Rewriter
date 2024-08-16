import os
filename = 'sym.txt'

addresses_rom = []


with open(filename, 'r') as file:
    lines = file.readlines()
    
    for line in lines:
        parts = line.split()
        if len(parts) > 1: 
            if(parts[1].startswith('f')):
                addresses_rom.append(parts[1].upper())


# Output the list of addresses
#print(addresses)
filename = 'build/windows/x64/debug/'

files_list = os.listdir(filename)
nodes_list = []
for file in files_list:
    if  file.startswith("node"):
        nodes_list.append(file)


analysed_addresses = []

for node in nodes_list:
    with open(filename+node, 'r') as file:
        lines = file.readlines()
        # Extracting the addresses from each line before the '|'
        for line in lines:
            address = line.split('|')[0].strip()
            if(address.startswith('0x')):
                analysed_addresses.append(address.replace("0x" , ""))

for analysed_addresse in analysed_addresses:
    if analysed_addresse in addresses_rom:
        occurence_number = addresses_rom.count(analysed_addresse) 
        for x in range(occurence_number):
            addresses_rom.remove(analysed_addresse)

print(addresses_rom)