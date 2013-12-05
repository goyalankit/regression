require 'pry'

Y = {}
feature_value = {}
line_count = 0
num_features = 0
File.open("mnist", "r").each_line do |line|
    if(line_count == 0)
        num_examples = line.split(" ")[0]
        num_features = line.split(" ")[1]
        line_count += 1
        next
    end
    line_count += 1
    split_values = line.split(" ")
    sample_number = line_count - 1
    split_values.each do |value, index|
        next if(value.strip == "")
        if(index == 0)
            Y[sample_number] = split_values[index]
            next
        end
        feature_number = value.split(":")[0]
        sample_value = value.split(":")[1]
        if(feature_value[feature_number].nil?)
            feature_value[feature_number] = { sample_number => sample_value }
        else
            feature_value[feature_number].merge!({ sample_number => sample_value })
        end
    end
end

p feature_value

num_features.to_i.times do |i|
    if(feature_value[i.to_s].nil?)
        #print "\n"
        next
    end
    feature_value[i.to_s].each do |sn, val|
        print "#{sn} #{val} "
    end
    print "\n"
end
