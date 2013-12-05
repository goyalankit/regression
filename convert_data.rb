Y = {}
feature_value = []
line_count = 0
num_features = 0
num_examples = 0
File.open("mnist", "r").each_line do |line|
    if(line_count == 0)
        num_examples = line.split(" ")[0]
        num_features = line.split(" ")[1]
        line_count += 1
        next
    end
    split_values = line.split(" ")
    sample_number = line_count - 1
    line_count += 1
    split_values.each_with_index do |value, index|
        next if(value.strip == "")
        if(index == 0)
            Y[sample_number] = split_values[index]
            next
        end
        feature_number = value.split(":")[0]
        sample_value = value.split(":")[1]
        if(feature_value[feature_number.to_i].nil?)
            feature_value[feature_number.to_i] = {sample_number => sample_value}
        else
            feature_value[feature_number.to_i][sample_number] = sample_value 
        end
    end
end

File.open("mnist_samples", 'a') do |file| 
    file.write("#{num_examples} #{num_features}") 

    num_features.to_i.times do |i|
        if(feature_value[i].nil?)
            file.write("\n")
            next
        end
        feature_value[i].each do |sn, val|
            file.write("#{sn} #{val} ")
        end
        file.write("\n")
    end
end

num_examples.to_i.times do |i|

    File.open("mnist_examples", 'a') { |file| file.write("#{Y[i]}\n") }
end
