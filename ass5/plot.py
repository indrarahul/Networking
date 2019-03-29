import os
ttl = [2, 8, 16]
for i in ttl:
	filename2= str(i) + ".csv"
	filename3= str(i) + ".png"
	for P in range(100,1001,100):
		filename1= str(P) + "-" + str(i) + ".csv"
		os.system("./clientPacketGen 127.0.0.1 4045 " + str(P) + " " + str(i) + " 70 " + filename1 +  " " + filename2) 
	os.system("gnuplot -c plot.sh " + filename2 + " " + filename3)
	os.system("truncate -s 0 " + filename2)
	os.system("xdg-open "+str(i)+".png")