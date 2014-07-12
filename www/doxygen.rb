class Doxygen < Middleman::Extension
  def initialize(app, opts = {}, &block)
    super
    app.after_build do |builder|
      builder.run 'rm -rf build/doc'
      builder.run 'cd .. && make doc'
      builder.run 'mv ../doc/html build/doc'
    end
  end
end

::Middleman::Extensions.register(:doxygen, Doxygen)
